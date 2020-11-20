#include "solver/solver.h"


namespace sudoku {


bool SolverSerial::SolverInternal(ProblemStateBase &problem_state) {

    PushChildren(problem_state);
    bool trial_suc = true;

    while (!stack_.Emtpy()) {
        Trial ret = stack_.Pop();
        SUDOKU_ASSERT(ret.val_ != UNFILLED);
        if (trial_suc) {
            problem_state.SanitiCheck();
            TakeSnapshot(ret.y_idx_, ret.x_idx_, problem_state);
        }

        trial_suc = problem_state.Set(ret.y_idx_, ret.x_idx_, ret.val_);

        // problem solved
        if (problem_state.CheckSolved()) {
            return true;
        }
        if (trial_suc) {
            // success, see if there are more things to fill
            PushChildren(problem_state);
        } else {
            // fail, backtrace
            if (!stack_.Emtpy()) {
                ret = stack_.Top();
                problem_state = ps_pool_.snapshot_arr_[ret.y_idx_][ret.x_idx_];
            }
        }
    }

    return problem_state.CheckSolved();
}

void SolverSerial::PushChildren(const ProblemStateBase &problem_state) {

    size_t x_idx, y_idx;
    Element val;
    bool ret = problem_state.GetIdxFixedByPeers(y_idx, x_idx, val);
    if (!problem_state.CheckValid()) {
        return;
    }
    
    if (ret) {
        Trial tmp = {x_idx, y_idx, val};
        stack_.Push(tmp);
    } else {
        size_t n_poss = problem_state.GetIdxWithMinPossibility(y_idx, x_idx);
        if (n_poss == 0)
            return;
        bool ret[N_NUM];
        n_poss = problem_state.GetConstraints(y_idx, x_idx, ret);

        for (size_t i = 1; i < N_NUM; ++i) {
            if (!ret[i]) {
                Trial tmp = {x_idx, y_idx, i};
                stack_.Push(tmp);
            }
        }
    }
}

void SolverSerial::TakeSnapshot(size_t y_idx, size_t x_idx, const ProblemStateBase &problem_state) {
    ProblemStateBase *snapshot = nullptr;
    ps_pool_.Apply(y_idx, x_idx, snapshot);
    *snapshot = problem_state;
}



}