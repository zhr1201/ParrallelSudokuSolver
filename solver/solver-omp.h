#ifndef SUDOKU_SOLVER_SOLVER_OMP_H_
#define SUDOKU_SOLVER_SOLVER_OMP_H_


#include "solver/solver.h"

namespace sudoku {


class SolverOmp : public SolverBase {
public:
    static SolverOmp* GetInstance();

protected:
    SolverOmp() {};
    static SolverOmp* singleton_;

private:
    virtual bool SolverInternal();

    DISALLOW_CLASS_COPY_AND_ASSIGN(SolverOmp);
};

}


#endif
