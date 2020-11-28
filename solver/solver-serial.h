#ifndef SUDOKU_SOLVER_SOLVER_SERIAL_H_
#define SUDOKU_SOLVER_SOLVER_SERIAL_H_


#include "solver/solver.h"

namespace sudoku {


class SolverSerial : public SolverBase {

public:
    SolverSerial() {};

private:
    virtual bool SolverInternal();

    DISALLOW_CLASS_COPY_AND_ASSIGN(SolverSerial);  // copy and asign will probably corrupt pointer arr
};

}


#endif