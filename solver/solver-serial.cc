#include "solver/solver.h"


namespace sudoku {


bool SolverSerial::SolverInternal() {

    PushChildren();
    bool trial_suc = true;

    while (!stack_.Emtpy()) {
        Trial ret = stack_.Pop();
        SUDOKU_ASSERT(ret.val_ != UNFILLED);
        if (trial_suc) {
            // problem_state.SanitiCheck();
            TakeSnapshot(ret.y_idx_, ret.x_idx_);
        }

        trial_suc = ps_.Set(ret.y_idx_, ret.x_idx_, ret.val_);

        // problem solved
        if (ps_.CheckSolved()) {
            return true;
        }
        if (trial_suc) {
            // success, see if there are more things to fill
            PushChildren();
        } else {
            // fail, backtrace
            if (!stack_.Emtpy()) {
                ret = stack_.Top();
                ps_ = ps_pool_.snapshot_arr_[ret.y_idx_][ret.x_idx_];
            }
        }
    }

    return ps_.CheckSolved();
}

void SolverSerial::PushChildren() {

    size_t x_idx, y_idx;
    Element val;
    bool ret = ps_.GetIdxFixedByPeers(y_idx, x_idx, val);
    if (!ps_.CheckValid()) {
        return;
    }
    
    if (ret) {
        Trial tmp = {x_idx, y_idx, val};
        stack_.Push(tmp);
    } else {
        size_t n_poss = ps_.GetIdxWithMinPossibility(y_idx, x_idx);
        if (n_poss == 0)
            return;
        bool ret[N_NUM];
        n_poss = ps_.GetConstraints(y_idx, x_idx, ret);

        for (size_t i = 1; i < N_NUM; ++i) {
            if (!ret[i]) {
                Trial tmp = {x_idx, y_idx, i};
                stack_.Push(tmp);
            }
        }
    }
}

void SolverSerial::TakeSnapshot(size_t y_idx, size_t x_idx) {
    ProblemStateBase *snapshot = nullptr;
    ps_pool_.Apply(y_idx, x_idx, snapshot);
    *snapshot = ps_;
}

bool SolverSerial::SetConstraint(size_t y_idx, size_t x_idx, Element val) {
    return ps_.Set(y_idx, x_idx, val);
}

}