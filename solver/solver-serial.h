#ifndef SUDOKU_SOLVER_SOLVER_SERIAL_H_
#define SUDOKU_SOLVER_SOLVER_SERIAL_H_


#include "solver/solver.h"

namespace sudoku {


class SolverSerial : public SolverBase {
public:
    static SolverSerial* GetInstance();

protected:
    SolverSerial() {};
    static SolverSerial* singleton_;

private:
    virtual bool SolverInternal();

    DISALLOW_CLASS_COPY_AND_ASSIGN(SolverSerial);  // copy and asign will probably corrupt pointer arr
};

}


#endif