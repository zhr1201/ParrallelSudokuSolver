#include "solver/solver-omp.h"
#include <vector>

namespace sudoku {


SolverOmp* SolverOmp::singleton_ = nullptr;


SolverOmp* SolverOmp::GetInstance() {
    if (singleton_ == nullptr) {
        singleton_ = new SolverOmp();
    }
    return singleton_;
}


bool SolverOmp::SolverInternal() {
    SUDOKU_ASSERT(sc_->GetStatus() != SolverCoreStatus::UNSET);

    Trial children[N_NUM];
    uint_t y_idx, x_idx;

    while (sc_->GetStatus() != SolverCoreStatus::FAILED &&
           sc_->GetStatus() != SolverCoreStatus::SUCCESS) {

        if (sc_->GetStatus() == SolverCoreStatus::LAST_TRY_FAILED) {
            // backtrace
            sc_->GetNextTryIdx(y_idx, x_idx);
            sc_->RecoverState(y_idx, x_idx);
            sc_->TryOneStep();

        } else {
            std::vector<uint_t> num = sc_->GetMultipleChildren(children);
            #pragma omp parallel for
                for (int i=1;i<N_NUM;i++) {
                    printf("Parallel Prunning\n");
                    if (num[i]) {
                        sc_->PushChildren(children, num[i]);
                        sc_->GetNextTryIdx(y_idx, x_idx);
                        sc_->TakeSnapshot(y_idx, x_idx);
                        sc_->TryOneStep();
                    }
                }
        }
    }

    return (sc_->GetStatus() == SolverCoreStatus::SUCCESS);
}


}
