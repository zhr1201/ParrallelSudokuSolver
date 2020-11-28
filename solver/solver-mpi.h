#ifndef SUDOKU_SOLVER_SOLVER_MPI_H_
#define SUDOKU_SOLVER_SOLVER_MPI_H_


#include "solver/solver.h"


namespace sudoku {

// used for SolverMPI, communicate info between different processes
// send side proxy, each process will have a proxy to talk to each one of the other process
class SolverSerialProxy {
public:
    SolverSerialProxy(size_t prox_idx);
    // most of them should be non-blocking and can't assume they will always succeed
    // initialize a problem
    void SetProblem(Solvable *problem);
    // set 
    void SetConstraint(size_t x_idx, size_t y_idx, Element val);
    // tell the node to stop
    void Kill();
    // ask for work, blocking call.
    // we only allow asking work from the rank that is one smaller than the current process
    // and we don't allow asking for mulitple times since this will introduce too much communication
    // overhead when it comes closer to a solution
    bool AskForWork();

private:
    const size_t prox_idx;
    DISALLOW_CLASS_COPY_AND_ASSIGN(SolverSerialProxy);
};


// should be running as another thread in each process
class SolverSerialProxyStub {
public:
    SolverSerialProxyStub(ProblemStateBase *tmp);

private:
    void SetProblem();
    void SetConstrait();
    void Kill();
    // if C ask work from B and B has finished all its work, we don't
    // allow B to ask work from A and pass it for B, cause it's also going to cause too mucch communication overhead
    void AskForWork(size_t prox_idx);
    DISALLOW_CLASS_COPY_AND_ASSIGN(SolverSerialProxyStub);
};


class MainProxy {
public:
    void SendResults(bool success, Validatable *rst);
private:
    DISALLOW_CLASS_COPY_AND_ASSIGN(MainProxy);
};


class MainProxyStub {
public:
    bool WaitAllResults();
private:
    bool GetResults();
    DISALLOW_CLASS_COPY_AND_ASSIGN(MainProxyStub);
};


class SolverMPI : public SolverBase {
    SolverMPI();
    ~SolverMPI();
private:
    virtual bool SolverInternal();
    static void* MasterStubThread();
    static void* SolverStubThread();

    MainProxy master_proxy_;
    MainProxyStub master_stub_;  // only used by the master process
    SolverSerialProxy *peer_proxy_;
    SolverSerialProxyStub this_solver_stub_;
};


}


#endif
