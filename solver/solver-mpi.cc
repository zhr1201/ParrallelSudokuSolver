// Implementation of solver-mpi (Haoran Zhou)

#include "solver/solver-mpi.h"

#include "solver/sudoku-problem.h"

// TODO!!!: TOO MESSY with all those ifndef. Probably should add a logger to avoid the ifndef NDEBUG macros

namespace sudoku {


SolverMPI *SolverMPI::singleton_ = nullptr;


void PrintMat(Element *problem) {
    for (uint_t i = 0; i < SIZE; ++i) {
        for (uint_t j = 0; j < SIZE; ++j) {
            std::cout << problem[IDX2OFFSET(i, j)];
        }
        std::cout << std::endl;
    }
}

void PrintTrials(Trial *arr, uint_t size) {
    for (uint_t i = 0; i < size; ++i) {
        std::cout << "y " << arr[i].y_idx_ << " x " << arr[i].x_idx_ << " val " << arr[i].val_ << std::endl;
    }
}


// doesn't support more than 16 workers cause communication overhead cross nodes would be way bigger
// than solving it using serial solver
inline void CheckValid(uint_t num_workers) {
    if (num_workers > MAX_WORKERS || num_workers < MIN_WORKERS) {
        throw std::runtime_error(
            "Only support num of workers smaller than 16 and greater than 2 (3 - 17 processes on total)\n");
    }
    uint_t i = num_workers;
    // power of 2 since work spliting here is like a binary tree
    while (i != 1) {
        if (i & 0x01) {
            throw std::runtime_error("only support num of workers (tot proc - 1) that is power of 2\n"); 
        }
        i = i >> 1;
    }
}

void WorkerProxy::PushProblem(Element *problem, Trial *trials, uint_t size, uint_t split) {

#ifndef NDEBUG
    int mpi_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
    std::cout << "Push from " << mpi_rank << " to worker " << prox_idx_ << std::endl;
    std::cout << "Push contents" << std::endl;
    PrintMat(problem);
    std::cout << "Init stack size " << size << std::endl;
    PrintTrials(trials, size);
#endif

    int header = PUSH_PROBLEM;
    
    uint_t counter = 0;
    memcpy(send_buffer_, &header, 4);  // header
    counter += 4;
    memcpy(send_buffer_ + counter, problem, MAT_SIZE);  // data mat
    counter += MAT_SIZE;
    memcpy(send_buffer_ + counter, &size, 4),  // size of initial stack
    counter += 4;
    for (uint_t i = 0; i < size; ++i) {
        trials[i].Serialize(send_buffer_ + counter);
        counter += TRIAL_SIZE;
    }
    memcpy(send_buffer_ + counter, &split, 4);  // splits
    counter += 4;
    MPI_Send(send_buffer_, counter, MPI_CHAR, prox_idx_, 0, MPI_COMM_WORLD);
}

void WorkerProxy::Stop() {
#ifndef NDEBUG
    int mpi_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
    std::cout << "Stop from " << mpi_rank << " to worker " << prox_idx_ << std::endl;
#endif
    int header = STOP;
    uint_t counter = 0;
    memcpy(send_buffer_, &header, 4);  // header 
    counter += 4;
    MPI_Send(send_buffer_, counter, MPI_CHAR, prox_idx_, 0, MPI_COMM_WORLD);
}

void WorkerProxy::Kill(bool suc) {
#ifndef NDEBUG
    int mpi_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
    std::cout << "Kill from " << mpi_rank << " to worker " << prox_idx_ << " with " << suc << std::endl;
#endif
    int header = KILL;
    uint_t counter = 0;
    memcpy(send_buffer_, &header, 4);  // header 
    counter += 4;
    memcpy(send_buffer_ + counter, &suc, 1);  // header 
    counter += 4;
    MPI_Send(send_buffer_, counter, MPI_CHAR, prox_idx_, 0, MPI_COMM_WORLD);
}

void WorkerStub::PushProblem(char *msg) {
    int mpi_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
    SUDOKU_ASSERT(worker_node_->stopped_);
    worker_node_->stopped_ = false;  // get up and work!

    Element problem[SIZE][SIZE];
    Trial trials[SIZE * SIZE * N_NUM];

    Element split_buffer[2][SIZE][SIZE];
    Trial trial_buffer[2][N_NUM * N_GRID];
    uint_t split_size1;
    uint_t split_size2;
    
    uint_t counter = 0;
    memcpy(problem, msg + counter, MAT_SIZE);  // data mat
    counter += MAT_SIZE;

#ifndef NDEBUG
    std::cout << "Get Push to " << mpi_rank << std::endl;
    PrintMat((Element*)problem);
#endif

    uint_t size;
    memcpy(&size, msg + counter, 4);  // size of initial stack

    counter += 4;
    for (uint_t i = 0; i < size; ++i) {
        trials[i].Deserialize(msg + counter);
        counter += TRIAL_SIZE;
    }
    uint_t split;
    memcpy(&split, msg + counter, 4);  // splits
    counter += 4;

#ifndef NDEBUG
    std::cout << "Rec stack size " << size << std::endl;
    PrintTrials(trials, size);
#endif

    Element *problem_tmp = (Element*)problem;
    Trial *trial_tmp = trials;
    uint_t size_tmp = size;
    SSudoku sudoku_problem(problem_tmp);
    sc_->SetProblem(sudoku_problem);
    sc_->PushChildren(trial_tmp, size_tmp);
    // divide problem and push to peers
    while (split != 1) {
        split = split >> 1;
        SSudoku sudoku_problem(problem_tmp);
        sc_->SetProblem(sudoku_problem);
        sc_->PushChildren(trial_tmp, size_tmp);
        worker_node_->SplitWorkToTwo((Element*)split_buffer[0], (Element*)split_buffer[1], (Trial*)trial_buffer[0],
                                      split_size1, (Trial*)trial_buffer[1], split_size2);
        std::vector<WorkerProxy> *worker_vec_prox = worker_node_->worker_proxy_vec_;
        worker_prox_iter iter = worker_vec_prox->begin();
        (iter + split + mpi_rank - 1)->PushProblem((Element*)split_buffer[1], (Trial*)trial_buffer[1], split_size2, split);
#ifndef NDEBUG
        std::cout << "split size 1 " << split_size1 << std::endl;
        std::cout << "split size 2 " << split_size2 << std::endl;
#endif
        problem_tmp = (Element*)split_buffer[0];
        trial_tmp = trial_buffer[0];
        size_tmp = split_size1;
    }
}

void WorkerStub::Stop() {
#ifndef NDEBUG
    int mpi_rank; 
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
    std::cout << "Get stop to " << mpi_rank << std::endl;
#endif
    worker_node_->stopped_ = true;
    sc_->Stop();
}


void WorkerStub::Kill(char *msg) {
    // stopped nodes will send a msg to the master
    // and once master noticed that all worker has stopped
    // it will kill all workers
#ifndef NDEBUG
    int mpi_rank; 
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
    SUDOKU_ASSERT(worker_node_->stopped_);
#endif
    bool ret;
    memcpy(&ret, msg, 1);

#ifndef NDEBUG
    std::cout << "Get kill to " << mpi_rank << " with" << ret << std::endl;
#endif

    worker_node_->global_suc_ = ret;
}


void MainProxy::SendResults(bool success, Element *rst) {
#ifndef NDEBUG
    int mpi_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
    std::cout << "Send results from " << mpi_rank << " to main" << std::endl;
    std::cout << "Success " << success << std::endl;
    if (success)
        PrintMat(rst);
#endif

    int header = SEND_RST;
    memcpy(send_buffer_, &header, 4);  // header
    memcpy(send_buffer_ + 4, &success, 1);  // bool sucess
    uint_t len = 8;
    if (success) {
        len += MAT_SIZE;
        memcpy(send_buffer_ + 8, rst, MAT_SIZE);  // data mat
    }
    MPI_Send(send_buffer_, len, MPI_CHAR, MASTER_RANK, 0, MPI_COMM_WORLD);
}

bool MainStub::SendResults(char *msg) {
    bool solved;
    memcpy(&solved, msg, sizeof(bool));

#ifndef NDEBUG
    std::cout << "Get result "<< std::endl;
    std::cout << "Solved " << solved << std::endl;
#endif

    if (solved) {
        memcpy(master_node_->rst_buffer_, msg + 4, sizeof(uint_t) * SIZE * SIZE);
#ifndef NDEBUG
        PrintMat((Element*)master_node_->rst_buffer_);
#endif
        return true;
    }
    return false;
}


bool Node::SplitWorkToTwo(Element *problem1, Element *problem2, Trial *trials1, uint_t &size1, Trial *trials2, uint_t &size2) {
    Trial children[N_NUM];
    Trial stack_contents[N_NUM * N_GRID];
    uint_t y_idx, x_idx;
    // split until just until exceeds the num of workers
    while (sc_->GetNumBranches() < 2) {
        if (sc_->GetStatus() == SolverCoreStatus::LAST_TRY_FAILED) {
            // backtrace
            sc_->GetNextTryIdx(y_idx, x_idx);
            sc_->RecoverState(y_idx, x_idx);

        } else {
            // LAST_TRY_SUCCESS or UNATTENPED
            uint_t num = sc_->GetChildren(children);
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
    uint_t num = sc_->GetTrialsInStack(stack_contents);
    SUDOKU_ASSERT(num >= 2);
    // split the root of the unsearched parts to another node cause this is probably going to split the work
    // more evenly
    uint_t root_branch_counter = 1;
    Trial first = stack_contents[0];
    for (uint_t i = 1; i < num; ++i) {
        Trial cur = stack_contents[i];
        if (first.y_idx_ != cur.y_idx_ || first.x_idx_ != cur.x_idx_)
            break;
        ++root_branch_counter;
    }
    uint_t first_branch_size = root_branch_counter / 2;

    uint_t second_branch_size = root_branch_counter - first_branch_size;
    size2 = second_branch_size;
    size1 = num - size2;
    
    for (uint_t i = 0; i < num; ++i) {
        if (i < second_branch_size) {
            trials2[i] = stack_contents[i];
        } else {
            trials1[i - second_branch_size] = stack_contents[i]; 
        }
    }

    sc_->GetElement(problem1);
    sc_->GetElementAtSnapshot(first.y_idx_, first.x_idx_, problem2);

    return true;
}

int Node::PreProcessMsg(char *msg_tot, char *&msg_body) {
    int msg_header;
    memcpy(&msg_header, msg_tot, sizeof(int)); 
    msg_body = msg_tot + 4;
    return msg_header;
}


bool MasterNode::Run() {
#ifndef NDEBUG
    std::cout << "Master node run" << std::endl;
#endif
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

    uint_t first_split_size;
    uint_t second_split_size;

    if (!SplitWorkToTwo((Element*)split_buffer[0], (Element*)split_buffer[1], (Trial*)trial_buffer[0],
                        first_split_size, (Trial*)trial_buffer[1], second_split_size)) {
        for (worker_prox_iter iter = worker_proxy_vec_->begin(); iter != worker_proxy_vec_->end(); ++iter) {
            iter->Stop();
        }
        return false;
    }

    uint_t split_workers = mpi_workers_ >> 1;
    worker_prox_iter iter = worker_proxy_vec_->begin();
    iter->PushProblem((Element*)split_buffer[0], (Trial*)trial_buffer[0], first_split_size, split_workers);
    (iter + split_workers)->PushProblem((Element*)split_buffer[1], (Trial*)trial_buffer[1], second_split_size, split_workers);
    return true;
}

bool MasterNode::CollectRst() {
    MPI_Status status;
    int finish_counter = 0;
    bool ret = false;
    while (finish_counter != mpi_workers_) {
        int flag = false;
        MPI_Iprobe(MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &flag, &status);
        if (flag) {
            ++finish_counter;
            int count;
            MPI_Get_count(&status, MPI_CHAR, &count);
            MPI_Recv(rec_buffer_, count, MPI_CHAR, status.MPI_SOURCE, 0, MPI_COMM_WORLD, &status);
            char *msg_body;
            int msg_header = PreProcessMsg(rec_buffer_, msg_body);
            SUDOKU_ASSERT(msg_header == SEND_RST);
            if (master_stub_.SendResults(msg_body)) {
                sc_->SetAnswer((Element*)rst_buffer_);
                ret = true;
            }
        }
    }
    worker_prox_iter iter = worker_proxy_vec_->begin();
    for (; iter != worker_proxy_vec_->end(); ++iter) {
        iter->Kill(ret);
    }
#ifndef NDEBUG
    std::cout << "Main returned " << ret << std::endl;
#endif
    return ret;
}


bool WorkerNode::Run() {
    // run untill killed by the master
#ifndef NDEBUG 
    std::cout << "worker node run" << std::endl;
#endif
    while (HandleRequest()) {
        // a problem was set or pushed by the master/worker
        if (!stopped_) {
            Solve();
            bool suc = (sc_->GetStatus() == SolverCoreStatus::SUCCESS);
            if (suc)
                sc_->GetElement((Element*)rst_buffer_);
            master_proxy_.SendResults(suc, (Element*)rst_buffer_);
        }
    }
#ifndef NDEBUG
    std::cout << "worker returned " << global_suc_ << std::endl;
#endif
    return global_suc_;
}

bool WorkerNode::Solve() {
    SUDOKU_ASSERT(sc_->GetStatus() != SolverCoreStatus::UNSET);
    // tmp vars for return
    Trial children[N_NUM];
    uint_t y_idx, x_idx;

    sc_->GetNextTryIdx(y_idx, x_idx);
    sc_->TakeSnapshot(y_idx, x_idx);
    sc_->TryOneStep();
    if ((sc_->GetStatus() == SolverCoreStatus::FAILED) |
        (sc_->GetNumBranches() == SolverCoreStatus::LAST_TRY_FAILED))
        return false;

    while (sc_->GetStatus() != SolverCoreStatus::FAILED &&
        sc_->GetStatus() != SolverCoreStatus::SUCCESS &&
        sc_->GetStatus() != SolverCoreStatus::KILLED) {
 
        if (sc_->GetStatus() == SolverCoreStatus::LAST_TRY_FAILED) {
            // backtrace
            sc_->GetNextTryIdx(y_idx, x_idx);
            sc_->RecoverState(y_idx, x_idx);

        } else {
            // LAST_TRY_SUCCESS or UNATTENPED
            uint_t num = sc_->GetChildren(children);
            sc_->PushChildren(children, num);
            sc_->GetNextTryIdx(y_idx, x_idx);
            sc_->TakeSnapshot(y_idx, x_idx);
        }
        HandleRequest();
        if (sc_->GetStatus() == SolverCoreStatus::KILLED)
            break;
        sc_->TryOneStep();
    }
    
    // find the answer, tell other worker node to stop
    if (sc_->GetStatus() == SolverCoreStatus::SUCCESS) {
        NotifyWorkerStop();
    }
    stopped_ = true;
    return (sc_->GetStatus() == SolverCoreStatus::SUCCESS);
}

void WorkerNode::NotifyWorkerStop() {
    worker_prox_iter iter = worker_proxy_vec_->begin();
    for (; iter != worker_proxy_vec_->end(); ++iter) {
        iter->Stop();
    }
}

bool WorkerNode::HandleRequest() {
    MPI_Status status;
    bool ret = true;
    int flag = 0;
    MPI_Iprobe(MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &flag, &status);
    if (flag) {
        int count;
        MPI_Get_count(&status, MPI_CHAR, &count);
        MPI_Recv(rec_buffer_, count, MPI_CHAR, status.MPI_SOURCE, 0, MPI_COMM_WORLD, &status);
        char *msg_body;
        int msg_header = PreProcessMsg(rec_buffer_, msg_body);
        switch (msg_header) {
            case PUSH_PROBLEM:
#ifndef NDEBUG
                std::cout << "Push received size " << count <<  std::endl;
#endif
                worker_stub_.PushProblem(msg_body);
                break;
            case STOP:
#ifndef NDEBUG
                std::cout << "Stop received size " << count << std::endl;
#endif
                worker_stub_.Stop();
                break;
            case KILL:
#ifndef NDEBUG
                std::cout << "Kill received size " << count << std::endl;
#endif
                worker_stub_.Kill(msg_body);
                ret = false;
                break;
            default:
#ifndef NDEBUG
                std::cout << "Unkown request " << msg_header << std::endl;
#endif
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
    bool ret = node_->Run();
    ++counter;
    return ret; 
}

}