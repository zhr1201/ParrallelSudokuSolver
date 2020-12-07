// MPI soduku solver (Haoran Zhou)

// one main MPI process to distribute work to worker processes
// using this class requires call MPI_Init at the begining of the main and MPI_finalized at the end


#ifndef SUDOKU_SOLVER_SOLVER_MPI_H_
#define SUDOKU_SOLVER_SOLVER_MPI_H_

#include <mpi.h>
#include <vector>

#include "solver/solver.h"

// similar to network programming where we have our own protocols for communication between all the processes

// types of MPI messages (message header should be 4 bytes (4 bytes memory aligned) for faster access)
// worker nodes receiving this message need to split the work and forward to its peers
#define PUSH_PROBLEM 0x00000000
// (4 bytes) header + (4 * SIZE * SIZE bytes) sudoku array + (4 byte) size of stack + (size of stack * 3 bytes) stack contents + (4 bytes) size to split
#define SET_PROBLEM 0x00000001
// (4 bytes) header + (4 * SIZE * SIZE bytes) sudoku array + (4 byte) size of stack + (size of stack * 3 bytes) stack contents
#define STOP 0x00000002
// (4 bytes) header: two-phase shutdown phase1: sent from any process to any other precesses, nofity them to stop, report to master and handle remaining requests
#define KILL 0x00000003
// (4 bytes) header + (4 bytes) bool sucess or not: two-phase shutdown phase2: sent from master to worker to tell them there are no remaining request to handle
// using only one phase could cause worker A was shut down and worker B used a blocking call to send msg to a, which will result in a deadlock.
#define ASK_FOR_WORK 0x00000004
// (4 bytes) header
#define SEND_RST 0x00000005
// (4 bytes) header + (4 bytes) if solved (packed boolean) + (4 * SIZE * SIZE bytes) sudoku array (not present if not solved) 
// report to master it has found the results or the subproblem has no solution or it was killed by other processes

#define MAX_WORKER_MSG_LEN (4 + 4 * SIZE * SIZE + 4 + TRIAL_SIZE * N_NUM * SIZE * SIZE + 4)
#define MAX_MAIN_MSG_LEN (4 + 4 + 4 * SIZE * SIZE)
#define MAT_SIZE (4 * SIZE * SIZE)
#define MASTER_RANK 0


namespace sudoku {


// used for SolverMPI, communicate info between different processes
// send side proxy, each process will have a proxy to talk to each one of the other process
// use Element **problem to represent a sudoku problem only for inside of solver-mpi
// send call are all blocking for buffer usage
class WorkerProxy {
public:
    WorkerProxy() {};
    void SetIdx(uint_t idx) { prox_idx_ = idx; };
    // initialize a problem, in respond to a AskForWork request, non_blocking
    // void SetProblem(Element **problem, Trial *trials, uint_t size);

    // difference is that push need to pass the problem to other nodes when necessary used in initial set up
    // if split is 1, there is no need to share the problem with another node
    // if split is not 1, upon receiving the problem, it is going to reduce the split to half, split problem to two and send one to the node that has a rank that
    //     is split + current_rank. repeat the process again until split reaches 1
    // e.g.
    //     node 1 receives a problem with size 8. 
    //     1) share the problem with 5 (split = 4)
    //     2) share the problem with 3 (split = 2)
    //     3) share the problem with 2 (split = 1)
    void PushProblem(Element *problem, Trial *trials, uint_t size, uint_t split);

    // tell the node to stop
    void Stop();
    void Kill(bool suc);
    // ask for work, blocking call.
    // we only allow asking work from the rank that is one smaller than the current process (wrap around)
    // and we don't allow asking for mulitple times since this will introduce too much communication
    // overhead when it comes closer to a solution
    // bool AskForWork();

    // notify the next worker it has existed and can't provide works
    // void NoitifyExit();

private:
    uint_t prox_idx_;
    char send_buffer_[MAX_WORKER_MSG_LEN];
    DISALLOW_CLASS_COPY_AND_ASSIGN(WorkerProxy);
};

using worker_prox_iter = std::vector<WorkerProxy>::iterator;
class MasterNode;
class WorkerNode;


class WorkerStub {
public:
    WorkerStub(SolverCore *sc, WorkerNode *node) : sc_(sc), worker_node_(node) {};
    // void SetProblem();
    void PushProblem(char* msg);
    void Stop();
    void Kill(char* msg);

private:
    // if C ask work from B and B has finished all its work, we don't
    // allow B to ask work from A and pass it for B, cause it's also going to cause too mucch communication overhead
    // void AskForWork();
    // void NotifyExit();

    SolverCore *sc_;
    WorkerNode *worker_node_;
    DISALLOW_CLASS_COPY_AND_ASSIGN(WorkerStub);
};


class MainProxy {
public:
    MainProxy() {};
    void SendResults(bool success, Element *rst);
private:
    char send_buffer_[MAX_MAIN_MSG_LEN];
    DISALLOW_CLASS_COPY_AND_ASSIGN(MainProxy);
};


class MainStub {
public:
    MainStub(SolverCore *sc, MasterNode *node) : sc_(sc), master_node_(node) {};
    bool SendResults(char *msg);
private:
    SolverCore *sc_;
    MasterNode *master_node_;
    DISALLOW_CLASS_COPY_AND_ASSIGN(MainStub);
};


class Node {
public:
    Node(uint_t mpi_procs, SolverCore *sc) : 
            mpi_procs_(mpi_procs), mpi_workers_(mpi_procs - 1),
            worker_proxy_vec_(new std::vector<WorkerProxy>(mpi_workers_)), sc_(sc){
        for (uint_t i = 0; i < mpi_workers_; ++i) {
            // mpi index one bigger than worker index
            (*worker_proxy_vec_)[i].SetIdx(i + 1);
        }
    };
    virtual ~Node() {};
    virtual bool Run() = 0;

protected:

    bool SplitWorkToTwo(Element *problem1, Element *problem2, Trial *trials1, uint_t &size1, Trial *trials2, uint_t &size2);
    // determine msg type and return a pointer to the msg body
    int PreProcessMsg(char *msg_tot, char *&msg_body);

    int mpi_procs_;
    int mpi_workers_;
    std::vector<WorkerProxy> *worker_proxy_vec_;
    SolverCore *sc_;
    DISALLOW_CLASS_COPY_AND_ASSIGN(Node);
};


class MasterNode : public Node {
    
public:
    MasterNode(uint_t mpi_procs, SolverCore *sc) : Node(mpi_procs, sc), master_stub_(sc, this) {};
    virtual ~MasterNode() {};
    virtual bool Run();

private:
    bool DistributeWork();
    bool CollectRst();

    MainStub master_stub_;
    Element rst_buffer_[SIZE][SIZE];

    char rec_buffer_[MAX_MAIN_MSG_LEN];
    friend class MainStub;
    DISALLOW_CLASS_COPY_AND_ASSIGN(MasterNode);
};


class WorkerNode : public Node {
public:
    WorkerNode(uint_t mpi_procs, SolverCore *sc) : 
            Node(mpi_procs, sc), worker_stub_(sc, this), stopped_(true), sc_(sc), global_suc_(false) {};
    virtual ~WorkerNode() {};

    virtual bool Run();

private:
    bool HandleRequest();  // return false when getting killed

    void NotifyWorkerStop();
    bool Solve();

    inline uint_t NextWorkerIdx() const;
    inline uint_t PrevWorkerIdx() const;

    WorkerStub worker_stub_;
    MainProxy master_proxy_;
    Element rst_buffer_[SIZE][SIZE];
    char rec_buffer_[MAX_WORKER_MSG_LEN];
    bool stopped_;
    SolverCore *sc_;
    bool global_suc_;

    friend class WorkerStub;
    DISALLOW_CLASS_COPY_AND_ASSIGN(WorkerNode);
};


class SolverMPI : public SolverBase {

protected:
    SolverMPI();
    static SolverMPI* singleton_;

public:
    static SolverMPI* GetInstance();
private:
    // need mpi barrier before calling this method for timing
    virtual bool SolverInternal();

    Node *node_;
    DISALLOW_CLASS_COPY_AND_ASSIGN(SolverMPI);
};


}


#endif
