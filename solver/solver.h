// sovler/problem-state.h (author: Haoran Zhou)

// Classes for solving soduku problems
// Avoid using stl containers and heap allocation for speed as problem-state.h has mentioned


#ifndef SUDOKU_SOLVER_SOLVER_H_
#define SUDOKU_SOLVER_SOLVER_H_


#include "solver/problem-state.h"
#include "util/global.h"
#include "itf/validatable-itf.h"


namespace sudoku {

class SudokuAnswer : public Validatable {
public:
    SudokuAnswer() : data_() {};
    virtual Element GetElement(size_t x_idx, size_t y_idx) const {
        return data_[y_idx][x_idx];
    }

private:
    Element data_[SIZE][SIZE];
    friend class SolverBase;  // only solver is allowed to modify the answer

    // shouldn't copy answer from others :)
    DISALLOW_CLASS_COPY_AND_ASSIGN(SudokuAnswer);
};


class SolverBase {

public:

    bool Solve(const Solvable &problem, SudokuAnswer &answer) {
        ProblemStateBase ps(&problem);
        if (!ps.CheckValid()) {
            return false;
        }
        if (!SolverInternal(ps))
            return false;
        
        // save answer
        for (size_t i = 0; i < SIZE; ++i) {
            for (size_t j = 0; j < SIZE; ++j) {
                answer.data_[i][j] = ps.Get(i, j);
            }
        }
        return true;
    }

    virtual ~SolverBase() {};

private:
    virtual bool SolverInternal(ProblemStateBase &problem_state) = 0;
};


// no need to worry about copying/creating ProblemState since it is super fast

class SolverSerial : public SolverBase {

    // record all trials and so that we can use stack to do backtracking
    struct Trial {
        size_t x_idx_;
        size_t y_idx_;
        Element val_;
    };

    class TrialStack {
        Trial trial_stack_[N_GRID * N_NUM];
        // stack pointer
        int sp_;
    public:
        TrialStack() : trial_stack_(), sp_(-1) {};
        Trial Top() {
            SUDOKU_ASSERT(sp_ >= 0);
            Trial ret;
            ret = {trial_stack_[sp_].x_idx_, trial_stack_[sp_].y_idx_, trial_stack_[sp_].val_};
            return ret;
        }
    
        Trial Pop() {
            Trial ret = Top();
            --sp_;
            return ret;
        };

        void Push(Trial trial) {
            trial_stack_[++sp_] = trial;
            SUDOKU_ASSERT(sp_ < N_GRID * N_NUM);
        }

        bool Emtpy() { return sp_ == -1; };
    };

    // a memory pool like data structure for snapshot
    struct ProblemStateMemPool {

        ProblemStateBase snapshot_arr_[SIZE][SIZE];

        ProblemStateMemPool() : snapshot_arr_() {};
        // only need apply since in a DFS style search, the state of the searched path don't need to be stored
        void Apply(size_t y_idx, size_t x_idx, ProblemStateBase *&ret) {
            ret = &snapshot_arr_[y_idx][x_idx];
        }

    };

public:
    SolverSerial() : ps_pool_(), stack_() {};

private:
    virtual bool SolverInternal(ProblemStateBase &problem_state);
    void PushChildren(const ProblemStateBase &problem_state);
    void TakeSnapshot(size_t y_idx, size_t x_idx, const ProblemStateBase &problem_state);

 

    // use snapshots of problem state for backtrace
    // (logical undo operation is very expensive and hard to implement so ProblemState doesn't support that)
    
    // points to a snapshot before filling a particular element
    ProblemStateMemPool ps_pool_;
    TrialStack stack_;

    DISALLOW_CLASS_COPY_AND_ASSIGN(SolverSerial);  // copy and asign will probably corrupt pointer arr
};


}


#endif