#include "solver/solver.h"


namespace sudoku {


Trial TrialStack::Top() {
    SUDOKU_ASSERT(sp_ >= 0);
    Trial ret;
    ret = {trial_stack_[sp_].x_idx_, trial_stack_[sp_].y_idx_, trial_stack_[sp_].val_};
    return ret;
}

Trial TrialStack::Pop()  {
    Trial ret = Top();
    --sp_;
    return ret;
}

void TrialStack::Push(Trial trial) {
    trial_stack_[++sp_] = trial;
    SUDOKU_ASSERT(sp_ < N_GRID * N_NUM);
}


void SolverCore::ProblemStateMemPool::Reset() {
    for (size_t i = 0; i < SIZE; ++i) {
        for (size_t j = 0; j < SIZE; ++j) {
            snapshot_set_[i][j] = false;
        }
    }
}

void SolverCore::SetProblem(const Solvable &problem) {
    ps_pool_.Reset();
    stack_.Reset();
    ps_.ResetProblem(&problem);
    status_ = SolverCoreStatus::UNATTEMPTED;
    if (!ps_.CheckValid()) {
        status_ = SolverCoreStatus::FAILED;
        return;
    }
    if (ps_.CheckSolved()) {
        status_ = SolverCoreStatus::SUCCESS;
        return;
    }
}


void SolverCore::SetProblem(const Solvable &problem) {
    ps_pool_.Reset();
    stack_.Reset();
    ps_.ResetProblem(&problem);
    status_ = SolverCoreStatus::UNATTEMPTED;
    if (!ps_.CheckValid()) {
        status_ = SolverCoreStatus::FAILED;
        return;
    }
    if (ps_.CheckSolved()) {
        status_ = SolverCoreStatus::SUCCESS;
        return;
    }
}


size_t SolverCore::GetChildren(Trial *trials) {
    SUDOKU_ASSERT(status_ == SolverCoreStatus::LAST_TRY_SUCCEED ||
                  status_ == SolverCoreStatus::UNATTEMPTED);
    size_t x_idx, y_idx;
    Element val;
    bool ret = ps_.GetIdxFixedByPeers(y_idx, x_idx, val);
    if (ret) {
        trials[0] = {x_idx, y_idx, val};
        return 1;
    } else {
        size_t n_poss = ps_.GetIdxWithMinPossibility(y_idx, x_idx);
        if (n_poss == 0)
            return 0;
        bool ret[N_NUM];
        n_poss = ps_.GetConstraints(y_idx, x_idx, ret);
        size_t counter = 0;
        for (size_t i = 1; i < N_NUM; ++i) {
            if (!ret[i]) {
                trials[counter] = {x_idx, y_idx, i};
                ++counter;
            }
        }
        SUDOKU_ASSERT(counter == n_poss);
        return counter;
    }
}

void SolverCore::PushChildren(const Trial *trails, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        stack_.Push(trails[i]);
    }
}

void SolverCore::TakeSnapshot(size_t y_idx, size_t x_idx) {
    ProblemStateBase *snapshot = nullptr;
    ps_pool_.Apply(y_idx, x_idx, snapshot);
    *snapshot = ps_;
}

bool SolverCore::SetConstraint(size_t y_idx, size_t x_idx, Element val) {
    // Set all the constraints in the problem state snapshot
    for (size_t i = 0; i < SIZE; ++i) {
        for (size_t j = 0; j < SIZE; ++j) {
            if (ps_pool_.snapshot_set_[i][j]) {
                ps_pool_.snapshot_arr_[i][j].SetConstraint(y_idx, x_idx, val);
            }
        }
    }

    ps_.SetConstraint(y_idx, x_idx, val);
    if (!ps_.CheckValid()) {
        status_ = SolverCoreStatus::LAST_TRY_FAILED;
        return false;
    }
    return true;
}

bool SolverCore::TryOneStep() {
    SUDOKU_ASSERT(status_ == SolverCoreStatus::LAST_TRY_SUCCEED ||
                  status_ == SolverCoreStatus::UNATTEMPTED);
    SUDOKU_ASSERT(!stack_.Emtpy());
    Trial ret = stack_.Pop();
    ps_.Set(ret.y_idx_, ret.x_idx_, ret.val_);

    if (ps_.CheckSolved()) {
        status_ = SolverCoreStatus::SUCCESS;
        return true;
    }

    if (!ps_.CheckValid()) {
        if (stack_.Emtpy()) 
            status_ = SolverCoreStatus::FAILED;
        else
            status_ = SolverCoreStatus::LAST_TRY_FAILED;
        return false;
    }

    status_ = SolverCoreStatus::LAST_TRY_SUCCEED;
    return true;
}

void SolverCore::GetNextTryIdx(size_t &y_idx, size_t &x_idx) {
    SUDOKU_ASSERT(!stack_.Emtpy());
    Trial tmp = stack_.Top();
    y_idx = tmp.y_idx_;
    x_idx = tmp.x_idx_;
}

void SolverCore::RecoverState(size_t y_idx, size_t x_idx) {
    SUDOKU_ASSERT(ps_pool_.snapshot_set_[y_idx][x_idx]);
    ps_ = ps_pool_.snapshot_arr_[y_idx][x_idx];
    status_ = SolverCoreStatus::LAST_TRY_SUCCEED;
}

void SolverCore::GetElementAtSnapshot(size_t y_idx, size_t x_idx, Element **ret) {
    SUDOKU_ASSERT(ps_pool_.snapshot_set_[y_idx][x_idx]);
    for (size_t i = 0; i < SIZE; ++i) {
        for (size_t j = 0; j < SIZE; ++j) {
            ret[i][j] = ps_pool_.snapshot_arr_[y_idx][x_idx].Get(i, j);
        }
    }
}

void SolverCore::GetElement(Element **ret) {
    for (size_t i = 0; i < SIZE; ++i) {
        for (size_t j = 0; j < SIZE; ++j) {
            ret[i][j] = ps_.Get(i, j);
        }
    }
}

size_t SolverCore::GetTrialsInStack(Trial *ret) {
    for (size_t i = 0; i < stack_.sp_ + 1; ++i) {
        ret[i] = stack_.trial_stack_[i];
    }
    return stack_.sp_ + 1;
}

void SolverCore::SetAnswer(Element** answer) {
    for (size_t i = 0; i < SIZE; ++i) {
        for (size_t j = 0; j < SIZE; ++j) {
            ps_.SetAnswer(i, j, answer[i][j]);
        }
    }
    status_ = SolverCoreStatus::SUCCESS;
}


bool SolverBase::Solve(const Solvable &problem, SudokuAnswer &answer) {
    sc_->SetProblem(problem);
    SolverCoreStatus status = sc_->GetStatus();
    if (status == SolverCoreStatus::SUCCESS)
        return true;
    if (status == SolverCoreStatus::FAILED)
        return false;
    if (!SolverInternal())
        return false;
    // save answer
    for (size_t i = 0; i < SIZE; ++i) {
        for (size_t j = 0; j < SIZE; ++j) {
            answer.data_[i][j] = sc_->ps_.Get(i, j);
        }
    }
    return true;
}

}