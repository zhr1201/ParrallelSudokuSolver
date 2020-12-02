#include "solver/solver-mpi.h"

#include "solver/sudoku-problem.h"

namespace sudoku {


SolverMPI *SolverMPI::singleton_ = nullptr;


// doesn't support more than 16 workers cause communication overhead cross nodes would be way bigger
// than solving it using serial solver
inline void CheckValid(size_t num_workers) {
    if (num_workers > MAX_WORKERS || num_workers < MIN_WORKERS) {
        throw std::runtime_error(
            "Only support num of workers smaller than 16 and greater than 2 (3 - 17 processes on total)\n");
    }
    // power of 2 since work spliting here is like a binary tree
    while (i != 1) {
        if (i && 0x01) {
            throw std::runtime_error("only support num of workers (tot proc - 1) that is power of 2\n"); 
        }
        i = i >> 2;
    }
}

void WorkerProxy::PushProlem(Element **problem, Trial *trials, size_t size, size_t split) {

    int header = PUSH_PROBLEM;
    
    size_t counter = 0;
    memcpy(send_buffer_, &header, 4);  // header
    counter += 4;
    memcpy(send_buffer_ + counter, problem, MAT_SIZE);  // data mat
    counter += MAT_SIZE;
    memcpy(send_buffer_ + counter, size, 4),  // size of initial stack
    counter += 4;
    for (size_t i = 0; i < size; ++i) {
        trials[i].Serialize(send_buffer_ + counter);
        counter += TRIAL_SIZE;
    }
    memcpy(send_buffer_ + counter, &split, 4);  // splits
    counter += 4;
    MPI_Send(send_buffer, counter, MPI_CHAR, prox_idx_, 0, MPI_COMM_WORLD);
}

void WokerPorxy::Stop() {
    int header = STOP;
    size_t counter = 0;
    memcpy(send_buffer_, &header, 4);  // header 
    counter += 4;
    MPI_Send(send_buffer_, counter, MPI_CHAR, prox_idx_, 0, MPI_COMM_WORLD);
}

void WokerPorxy::Kill() {
    int header = KILL;
    size_t counter = 0;
    memcpy(send_buffer_, &header, 4);  // header 
    counter += 4;
    MPI_Send(send_buffer_, counter, MPI_CHAR, prox_idx_, 0, MPI_COMM_WORLD);
}

void WorkerStub::PushProblem(char *msg) {
    SUDOKU_ASSERT(!stopped);
    stopped_ = false;  // get up and work!

    Element problem[SIZE][SIZE];
    Trial trials[SIZE * SIZE * N_NUM];

    Element split_buffer[2][SIZE][SIZE];
    Trial split_trial_buffer[2][N_NUM * N_GRID];
    size_t split_size1;
    size_t split_size2;
    
    size_t counter = 0;
    memcpy(problem, msg + counter, MAT_SIZE);  // data mat
    counter += MAT_SIZE;
    size_t size;
    memcpy(size, msg + counter, 4),  // size of initial stack
    counter += 4;
    for (size_t i = 0; i < size; ++i) {
        trials[i].Deserialize(send_buffer_ + counter);
        counter += TRIAL_SIZE;
    }
    size_t split;
    memcpy(&split, send_buffer_ + counter, 4);  // splits
    counter += 4;

    Element **problem_tmp = problem;
    Trial *trial_tmp = trials;
    size_t size_tmp = size;
    // divide problem and push to peers
    while (split != 1) {
        split = split >> 2;
        SSudoku sudoku_problem(problem_tmp);
        sc_->SetProblem(sudoku_problem);
        sc_->PushChildren(trials, size_tmp);
        node_->SplitWorkToTwo(split_buffer[0], split_buffer[1], trial_buffer[0],
                              trial_buffer[1], split_size1, split_size2);
        std::vector<WorkerProxy> *worker_vec_prox = worker_node_->worker_proxy_vec_;
        worker_prox_iter iter = worker_vec_prox->begin();
        (iter + split)->PushProblem(split_buffer[1], split_trial_buffer[1], split_size2, split);
        
        problem_tmp = split_buffer[0];
        trial_tmp = trial_buffer[0];
        size_tmp = split_size1;
    }
}

void WorkerStub::Stop() {
    stopped_ = true;
    sc_->Stop();
}


void WorkerStub::Kill() {
    // stopped nodes will send a msg to the master
    // and once master noticed that all worker has stopped
    // it will kill all workers
    SUDOKU_ASSERT(stopped_);
}


bool MainProxy::SendResults(bool success, Element **rst) {
    int header = SEND_RST;
    memcpy(send_buffer_, &header, 4);  // header
    memcpy(send_buffer_ + 4, &success, 1);  // bool sucess
    size_t len = 8;
    if (success) {
        len += MAT_SIZE;
        memcpy(send_buffer_ + 8, *rst, MAT_SIZE);  // data mat
    }
    MPI_Send(send_buffer_, len, MPI_CHAR, MASTER_RANK, 0, MPI_COMM_WORLD);
}

bool MainStub::SendResults(char *msg) {
    bool solved;
    memcpy(&solved, msg, sizeof(bool));
    if (solved) {
        memcpy(master_node_->rst_buffer_, msg + 4, sizeof(size_t) * SIZE * SIZE);
        return true;
    }
    return false;
}


bool Node::SplitWorkToTwo(Element **problem1, Element **problem2, Trial *trials1, size_t &size1, Trials *trials2, size_t &size2) {
    Trial children[N_NUM];
    Trial stack_contents[N_NUM * N_GRID];
    size_t y_idx, x_idx;
    int count = 0;
    // split until just until exceeds the num of workers
    while (sc_->GetNumBranches() < 2) {
        if (sc_->GetStatus() == SolverCoreStatus::LAST_TRY_FAILED) {
            // backtrace
            sc_->GetNextTryIdx(y_idx, x_idx);
            sc_->RecoverState(y_idx, x_idx);

        } else {
            // LAST_TRY_SUCCESS or UNATTENPED
            size_t num = sc_->GetChildren(children);
            sc_->PushChildren(children, num);
            sc_->GetNextTryIdx(y_idx, x_idx);
            sc_->TakeSnapshot(y_idx, x_idx);
        }
        if (sc_->GetStatus() == SolverCoreStatus::SUCCESS || sc_->GetStatus() == SolverCoreStatus::FAILED) {
            return false;
        }
    }
    // succeed or find problem unsolvable during split
    if (sc_->GetStatus() == SolverCoreStatus::FAILED || sc_->GetStatus() == SolverCoreStatus::SUCCESS) {
        return false;
    }
    // each returned value means one direction to try, and generally the element with bigger index is
    // more deep in the search tree and requires less amount of work to complete
    // so first we try to split the work evenly and assign the remaining the the last partition
    size_t num = sc_->GetTrialsInStack(stack_contents);
    SUDOKU_ASSERT(num >= 2);
    // split the root of the unsearched parts to another node cause this is probably going to split the work
    // more evenly
    size_t root_branch_counter = 1;
    Trial first = stack_contents[0];
    for (size_t i = 1; i < num; ++i) {
        Trial cur = stack_contents[i];
        if (first.y_idx_ != cur.y_idx_ || first.x_idx_ != cur.x_idx_)
            break;
        ++root_branch_counter;
    }
    size_t first_branch_size = root_branch_counter / 2;

    size_t second_branch_size = root_branch_counter - first_branch_size;
    size2 = second_branch_size;
    size1 = num - size2;
    
    for (size_t i = 0; i < num; ++i) {
        if (i < second_branch_size) {
            trials1[i] = stack_contents[i]
        } else {
            trials2[i - second_branch_size] = stack_contents[i]; 
        }
    }

    sc_->GetElement(problem1);
    sc_->GetElementAtSnapshot(first.y_idx_, first.x_idx_, problem2);

    return true;
}

int Node::PreProcessMsg(char *msg_tot, char &*msg_body) {
    int msg_header;
    memcpy(&msg_header, msg_tot, sizeof(int)); 
    msg_body = msg_tot + 4;
    return msg_header;
}


bool MasterNode::Run() {
    if (!DistributeWork()) {
        if (sc_->GetStatus() == SolverCoreStatus::SUCCESS)
            return true;
        else
            return false;
    }
    return CollectRst();
}

bool MasterNode::DistributeWork() {
    Element split_buffer[2][SIZE][SIZE];
    Trial trial_buffer[2][N_NUM * N_GRID];

    size_t first_split_size;
    size_t second_split_size;

    if (!SplitWorkToTwo(split_buffer[0], split_buffer[1], trial_buffer[0],
                        trial_buffer[1], first_split_size, second_split_size)) {
        for (worker_prox_iter iter = worker_proxy_->begin(); iter != worker_proxy_->end(); ++iter) {
            iter->Stop();
        }
        return false;
    }
    size_t split_workers = mpi_workers_ >> 2;
    worker_prox_iter iter = worker_proxy_->begin();
    iter->PushProlem(split_buffer[0], trial_buffer[0], split_workers);
    (iter + split_workers)->PushProblem(split_buffer[1], trial_buffer[1], split_workers);
    return true;
}

bool MasterNode::CollectRst() {
    MPI_Status status;
    int finish_counter = 0;
    bool ret = false;
    while (finish_counter != mpi_workers_) {
        bool flag = false;
        MPI_Iprobe(MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &flag, &status);
        if (flag) {
            ++finish_counter;
            int count;
            MPI_Get_count(&status, MPI_CHAR, &count);
            MPI_Recv(rec_buffer_, count, MPI_CHAR, status.MPI_SOURCE, 0, MPI_COMM_WORLD, &status);
            char *msg_body;
            int msg_header = PreProcessMsg(rec_buffer_, msg_body);
            SUDOKU_ASSERT(msg_header == SEND_RST)
            if(master_stub_.SendResults(msg_body)) {
                sc_->SetAnswer(rst_buffer_);
                ret = true;
            }
        }
    }

    worker_prox_iter iter = worker_proxy_vec_->begin();
    for (; iter != worker_proxy_vec_->end(); ++iter) {
        iter->Kill();
    }
    return ret;
}


bool WorkerNode::Run() {
    // run untill killed by the master
    while (HandleRequests()) {
        // a problem was set or pushed by the master/worker
        if (!stopped_) {
            Solve();
        }
    }; 
    return (sc_->GetStatus() == SolverCoreStatus::SUCCESS);
}

bool WorkerNode::Solve() {
    SUDOKU_ASSERT(sc_->GetStatus() != SolverCoreStatus::UNSET);
    // tmp vars for return
    Trial children[N_NUM];
    size_t y_idx, x_idx;

    while (sc_->GetStatus() != SolverCoreStatus::FAILED &&
        sc_->GetStatus() != SolverCoreStatus::SUCCESS &&
        sc_->GetStatus()) != SolverCoreStatus::KILLED) {

        if (sc_->GetStatus() == SolverCoreStatus::LAST_TRY_FAILED) {
            // backtrace
            sc_->GetNextTryIdx(y_idx, x_idx);
            sc_->RecoverState(y_idx, x_idx);

        } else {
            // LAST_TRY_SUCCESS or UNATTENPED
            size_t num = sc_->GetChildren(children);
            sc_->PushChildren(children, num);
            sc_->GetNextTryIdx(y_idx, x_idx);
            sc_->TakeSnapshot(y_idx, x_idx);
        }
        HandleRequest();
        sc_->TryOneStep();
    }
    
    // find the answer, tell other worker node to stop
    if (sc_->GetStatus() == SolverCoreStatus::SUCCESS) {
        NotifyWorkerStop();
    } 
}

void WorkerNode::NotifyWorkerStop() {
    worker_prox_iter iter = worker_proxy_vec_->begin();
    for (; iter != worker_proxy_vec_->end(); ++iter) {
        iter->Stop();
    }
}

bool WorkerNode::HanldeRequest() {
    MPI_Status status;
    bool ret = true;
    bool flag = false;
    MPI_Iprobe(MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &flag, &status);
    if (flag) {
        int count;
        MPI_Get_count(&status, MPI_CHAR, &count);
        MPI_Recv(rec_buffer_, count, MPI_CHAR, status.MPI_SOURCE, 0, MPI_COMM_WORLD, &status);
        char *msg_body;
        int msg_header = PreProcessMsg(rec_buffer_, msg_body);
        switch (msg_header) {
            case PUSH_PROBLEM:
                worker_stub_.PushProblem(msg_body);
                break;
            case STOP:
                worker_stub_.Stop();
                break;
            case KILL:
                worker_stub_.Kill();
                ret = false;
                break;
            default:
                SUDOKU_ASSERT(false);  // shouldn't reach here
        }
    }
    return ret;
}

SolverMPI* SolverMPI::GetInstance() {
    if (singleton_ == nullptr) {
        singleton_ = new SolverMPI();
    }
    return singleton_;
}

SolverMPI::SolverMPI() {
    int mpi_procs;
    int mpi_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);

    int mpi_workers = mpi_procs - 1;
    CheckValid(mpi_workers);

    if (mpi_rank == 0)
        node_ = new MasterNode(mpi_procs, sc_);
    else
        node_ = new WorkerNode(mpi_procs, sc_);
}

bool SolverMPI::SolverInternal() {
    // only support calling this function once at this point
    static int counter = 0;
    SUDOKU_ASSERT(counter == 0);
    node_->Run();
    ++counter;
}


bool SolverMPI::RunWorker() {
    worker_prox_iter iter = worker_proxy_->begin();
    if (!GetInitSetup()) {
        // (iter + NextWorkerIdx())->NoitifyExit();
        HanldeRequestAfterStop();
    } else {
        bool ret = WorkerMain();
        if (ret) {
            for (; iter != worker_proxy_->end(); ++iter) {
                iter->Stop();
            }
            sc_->GetElement(rst_buffer_);
            master_proxy_.SendResults(true, rst_buffer_);
        } else {
            master_proxy_.SendResults(false);
        }
        return ret;
    }
}

bool GetInitSetup() {
    MPI_Status status;
    MPI_Probe(MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &flag, &status);
    
}

bool SolverMPI::WorkerMain() {
    Trial children[N_NUM];
    size_t y_idx, x_idx;

    while (sc_->GetStatus() != SolverCoreStatus::FAILED &&
           sc_->GetStatus() != SolverCoreStatus::SUCCESS &&
           sc_->GetStatus() != SolverCoreStatus::KILLED) {

        if (sc_->GetStatus() == SolverCoreStatus::LAST_TRY_FAILED) {
            // backtrace
            sc_->GetNextTryIdx(y_idx, x_idx);
            sc_->RecoverState(y_idx, x_idx);

        } else {
            // LAST_TRY_SUCCESS or UNATTENPED
            size_t num = sc_->GetChildren(children);
            sc_->PushChildren(children, num);
            sc_->GetNextTryIdx(y_idx, x_idx);
            sc_->TakeSnapshot(y_idx, x_idx);
        }
        HandleRequest2Worker();
        sc_->TryOneStep();
    }

    return (sc_->GetStatus() == SolverCoreStatus::SUCCESS);
}

size_t SolverMPI::NextWorkerIdx() const {
    size_t worker_idx = mpi_rank_;
    return （worker_idx == mpi_workers_） ? 0 : worker_idx;
}


size_t SolverMPI::PrevWorkerIdx() const {
    return worker_idx == 0 ? mpi_workers_ - 1 : worker_idx - 1;
}


}