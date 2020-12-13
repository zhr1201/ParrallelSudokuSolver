// sovler/problem-state.h (author: Haoran Zhou)

// Classes for solving soduku problems
// Avoid using stl containers and heap allocation in the solving phase for speed as problem-state.h has mentioned


#ifndef SUDOKU_SOLVER_SOLVER_H_
#define SUDOKU_SOLVER_SOLVER_H_


#include "solver/problem-state.h"
#include "util/global.h"
#include "itf/validatable-itf.h"


namespace sudoku {


enum SolverCoreStatus {
    UNSET = 0,
    UNATTEMPTED = 1,
    LAST_TRY_SUCCEED = 2,
    LAST_TRY_FAILED = 3,
    FAILED = 4,
    SUCCESS = 5,
    // one solution find by other workers
    KILLED = 6
};


class SudokuAnswer : public Validatable {
public:
    SudokuAnswer() : data_() {};
    virtual Element GetElement(uint_t x_idx, uint_t y_idx) const { return data_[y_idx][x_idx]; }
private:
    Element data_[SIZE][SIZE];
    friend class SolverBase;  // only solver is allowed to modify the answer
    // shouldn't copy answer from others :)
    DISALLOW_CLASS_COPY_AND_ASSIGN(SudokuAnswer);
};


// record all trials and so that we can use stack to do backtracking
# define TRIAL_SIZE 12

struct Trial {
    unsigned int x_idx_;
    unsigned int y_idx_;
    Element val_;

    void Serialize(char *buf) {
        memcpy(buf, &x_idx_, 4);
        memcpy(buf + 4, &y_idx_, 4);
        memcpy(buf + 8, &val_, 4);

    }

    void Deserialize(const char *buf) {
        memcpy(&x_idx_, buf, 4);
        memcpy(&y_idx_, buf + 4, 4);
        memcpy(&val_, buf + 8, 4);
    }
};


class TrialStack {
public:
    TrialStack() : trial_stack_(), sp_(-1) {};
    Trial Top();
    Trial Pop();
    void Push(Trial trial);
    bool Emtpy() { return sp_ == -1; };
    void Reset() { sp_ = -1; };
    uint_t Size() { return sp_ + 1; }
    // caller responsible for proving enough space
private:
    Trial trial_stack_[N_GRID * N_NUM];
    int sp_;  // stack pointer
    friend class SolverCore;
};


// logic of searching (with prunning)
// no need to worry about copying/creating ProblemState since it is super fast
// decouple the logic of basic searching from a monolithic serial solver so that
// some methods can be used by other parrallel solvers
// probably going to make it a little big less efficient for the serial solver
class SolverCore {
    // a memory pool like data structure for snapshot
    struct ProblemStateMemPool {

        ProblemStateBase snapshot_arr_[SIZE][SIZE];
        bool snapshot_set_[SIZE][SIZE];

        ProblemStateMemPool() : snapshot_arr_(), snapshot_set_() {};
        // only need apply method since in a DFS style search, the state of the searched path doesn't need to be stored
        // so we don't need to return the unused ones, we just need to overwrite the contents
        void Apply(uint_t y_idx, uint_t x_idx, ProblemStateBase *&ret) {
            ret = &snapshot_arr_[y_idx][x_idx];
            snapshot_set_[y_idx][x_idx] = true;
        }
        void Reset();
    };

public:
    SolverCore() : ps_pool_(), stack_(), status_(SolverCoreStatus::UNSET) {};
    void SetProblem(const Solvable &problem);

    // get the element with the min possiblities
    uint_t GetChildren(Trial *trials);
    uint_t * GetMultipleChildren(Trial *trials);
    // push into the DFS stack
    // used for 1. DFS search 2. force the solver to search in a particular direction (set problem in another process)
    void PushChildren(const Trial *trails, uint_t len);
    // Use the top of the stack to do one trial
    void GetNextTryIdx(uint_t &y_idx, uint_t &x_idx);
    bool TryOneStep();
    SolverCoreStatus GetStatus() { return status_; };

    // use snapshots of problem state for backtrace
    // (logical undo operation is very expensive and hard to implement so ProblemState won't support that)
    // take a snapshot of ProblemState before filling the entry at y_idx and x_idx
    void TakeSnapshot(uint_t y_idx, uint_t x_idx);
    // recover to the state before trying to fill the element at y_idx, x_idx
    void RecoverState(uint_t y_idx, uint_t x_idx);

    // useful for splitting work, returns a 2 D sudoku mat
    void GetElementAtSnapshot(uint_t y_idx, uint_t x_idx, Element *ret);
    void GetElement(Element *ret);
    // get the current number of feasible branches of the DFS tree, sizeof the stack
    // useful for splitting the work into a fixed number of subproblems
    uint_t GetNumBranches() { return stack_.Size(); };
    uint_t GetTrialsInStack(Trial *ret);

    // could be used by parallel solver, setting a constriant discovered by another process
    bool SetConstraint(uint_t y_idx, uint_t x_idx, Element val);

    // only used when you know it's the correct answer for sure
    // (another process solved the problem and tell you the answer)
    void SetAnswer(Element* answer);
    void Stop() { status_ = SolverCoreStatus::KILLED; };
    bool TrialStackEmpty() { return stack_.Emtpy(); };

private:

    // points to a snapshot before filling a particular element
    ProblemStateMemPool ps_pool_;
    TrialStack stack_;
    ProblemStateBase ps_;
    SolverCoreStatus status_;
    friend class SolverBase;

};


class SolverBase {

public:
    // we can use new here cause solver will be initailized at the begining and will be
    // reused for each problem
    SolverBase() : sc_(new SolverCore()){};
    bool Solve(const Solvable &problem, SudokuAnswer &answer);
    virtual ~SolverBase() {};

private:
    virtual bool SolverInternal() = 0;

protected:
    SolverCore *sc_;

};

}

#endif
