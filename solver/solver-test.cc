#include <vector>
#include "util/timer.h"
#include "util/global.h"
#include "solver/problem-state.h"
#include "util/string-utils.h"
#include "solver/solver-serial.h"
#include "validator/validator.h"
#include "solver/sudoku-problem.h"


namespace sudoku {


void TestProblemStateSolved() {
    const std::string in_csv = "test-data-1.csv";
    SSudoku ts(in_csv);
    Timer tm;
    ProblemStateBase ps(&ts);
    std::cout << "Take " << tm.Elapsed() << " to set up ProblemState from a solved sudoku" << std::endl;
    assert(ps.CheckSolved());
}

void TestProblemStateFixedByConstrant() {
    const std::string in_csv = "test-data-2.csv";
    SSudoku ts(in_csv);
    ProblemStateBase ps(&ts);
    uint_t x_idx, y_idx;
    uint_t n_pos = ps.GetIdxWithMinPossibility(x_idx, y_idx);
    assert(n_pos == 1);
    assert(x_idx == 0);
    assert(y_idx == 0);
}

void TestProblemStateFixedByPeers() {
    const std::string in_csv = "test-data-3.csv";
    SSudoku ts(in_csv);
    ProblemStateBase ps(&ts);
    uint_t x_idx, y_idx;
    Element val;
    bool ret = ps.GetIdxFixedByPeers(x_idx, y_idx, val);
    assert(ret);
    assert(val == 3);
    assert(x_idx == 0);
    assert(y_idx == 0); 
}

void TestProblemStateCopyConstructer() {
    const std::string in_csv = "test-data-4.csv";
    SSudoku ts(in_csv);
    ProblemStateBase ps(&ts);
    Timer tm;
    ProblemStateBase ps_copy(ps);
    std::cout << "Take " << tm.Elapsed() << " to finish copy constructor" << std::endl;
    assert(!ps_copy.CheckSolved());
    ps_copy.Set(8, 7, 7);

    uint_t y_idx, x_idx;
    Element ele;
    assert(ps_copy.GetIdxFixedByPeers(y_idx, x_idx, ele));
    assert(y_idx == 8);
    assert(x_idx == 8);
    assert(ele == 5);

    ps_copy.Set(8, 8, 5);
    assert(ps_copy.CheckSolved());
    assert(!ps.CheckSolved());
}

void TestProblemStateAssignOps() {
    const std::string in_csv = "test-data-5.csv";
    SSudoku ts(in_csv);
    ProblemStateBase ps(&ts);

    ProblemStateBase ps2 = ps;
    uint_t x_idx, y_idx;
    Element val;
    bool ret = ps2.GetIdxFixedByPeers(x_idx, y_idx, val);
    assert(!ret);

    ps2.Set(1, 6, 3);
    ps2.Set(6, 2, 3);
    ret = ps2.GetIdxFixedByPeers(x_idx, y_idx, val);
    assert(ret);
    assert(val == 3);
    assert(x_idx == 0);
    assert(y_idx == 0); 
}


// three regular problem that can be solved
void TestSerialSolver() {
    const std::string in_csv = "test-data-6.csv";
    SSudoku ts(in_csv);
    SolverSerial *ss = SolverSerial::GetInstance();
    SudokuAnswer answer;
    
    Timer tm;
    bool ret = ss->Solve(ts, answer);
    std::cout << "Take " << tm.Elapsed() << " to solve" << std::endl;

    assert(ret);

    Validator val;
    assert(val.Validate(&answer, &ts));
}


void TestSerialSolver2() {
    const std::string in_csv = "test-data-7.csv";
    SSudoku ts(in_csv);
    SolverSerial *ss = SolverSerial::GetInstance();
    SudokuAnswer answer;
    
    Timer tm;
    bool ret = ss->Solve(ts, answer);
    std::cout << "Take " << tm.Elapsed() << " to solve" << std::endl;

    assert(ret);

    Validator val;
    assert(val.Validate(&answer, &ts));
}

void TestSerialSolver3() {
    const std::string in_csv = "test-data-8.csv";
    SSudoku ts(in_csv);
    SolverSerial *ss = SolverSerial::GetInstance();
    SudokuAnswer answer;
    Timer tm;
    bool ret = ss->Solve(ts, answer);
    std::cout << "Take " << tm.Elapsed() << " to solve" << std::endl;

    assert(ret);

    Validator val;
    assert(val.Validate(&answer, &ts));
}

void TestSerialSolver4() {
    // solve two
    const std::string in_csv = "test-data-7.csv";
    SSudoku ts(in_csv);
    SolverSerial *ss = SolverSerial::GetInstance();
    SudokuAnswer answer;
    bool ret = ss->Solve(ts, answer);

    assert(ret);

    Validator val;
    assert(val.Validate(&answer, &ts));
    
    const std::string in_csv2 = "test-data-8.csv";
    SSudoku ts2(in_csv2); 
 
    ret = ss->Solve(ts2, answer);
    assert(ret);
    assert(val.Validate(&answer, &ts2));
}

void TestSerialSolver5() {
    // no answer
    const std::string in_csv = "test-data-9.csv";
    SSudoku ts(in_csv);
    SolverSerial *ss = SolverSerial::GetInstance();
    SudokuAnswer answer;
    bool ret = ss->Solve(ts, answer);
    assert(!ret);
}

}

int main() {
    using namespace sudoku;
    TestProblemStateSolved();
    TestProblemStateFixedByConstrant();
    TestProblemStateFixedByPeers();
    TestProblemStateCopyConstructer();
    TestProblemStateAssignOps();
    TestSerialSolver();
    TestSerialSolver2(); 
    TestSerialSolver3();
    TestSerialSolver4();
    TestSerialSolver5();
    return 0;
}
