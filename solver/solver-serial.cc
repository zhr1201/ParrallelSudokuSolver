#include "solver/solver-serial.h"


namespace sudoku {


SolverSerial* SolverSerial::singleton_ = nullptr;


SolverSerial* SolverSerial::GetInstance() {
    if (singleton_ == nullptr) {
        singleton_ = new SolverSerial();
    }
    return singleton_;
}


bool SolverSerial::SolverInternal() {
    SUDOKU_ASSERT(sc_->GetStatus() != SolverCoreStatus::UNSET);
    // tmp vars for return
    Trial children[N_NUM];
    uint_t y_idx, x_idx;

    while (sc_->GetStatus() != SolverCoreStatus::FAILED &&
           sc_->GetStatus() != SolverCoreStatus::SUCCESS) {

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

        sc_->TryOneStep();
    }

    return (sc_->GetStatus() == SolverCoreStatus::SUCCESS);
}


}