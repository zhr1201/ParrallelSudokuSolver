#include <vector>
#include "util/global.h"
#include "util/timer.h"
#include "solver/problem-state.h"
#include "util/string-utils.h"
#include "solver/solver-omp.h"
#include "validator/validator.h"
#include "solver/sudoku-problem.h"


namespace sudoku {


void TestSolverOmp() {
    const std::string in_csv = "test-data-7.csv";
    //printf("load testing file: %s\n", in_csv.c_str());
    //SSudoku ts(in_csv);
    //SolverOmp *ss = SolverOmp::GetInstance();
    //SudokuAnswer answer;
    //bool ret = ss->Solve(ts, answer);
    //assert(ret != 0);
}

}

int main() {
    using namespace sudoku;
    TestSolverOmp();
    return 0;
}
