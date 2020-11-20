#include <vector>
#include "util/timer.h"
#include "util/global.h"
#include "solver/problem-state.h"
#include "util/string-utils.h"
#include "solver/solver.h"


namespace sudoku {

class TestSudoku : public Solvable {
public:
    virtual Element GetElement(size_t x_idx, size_t y_idx) const {
        return data_[y_idx][x_idx];
    }

    void ReadFromCSV(const std::string &csv_file) {
        std::ifstream file(csv_file);
        std::string line;
        assert(file.is_open());

        for (size_t i = 0; i < SIZE; ++i) {

            std::getline(file, line);
            std::vector<std::string> sep_num = split(line, ",");
            data_.push_back(std::vector<Element>());
            assert(sep_num.size() == SIZE);
            for (size_t j = 0; j < SIZE; ++j) {
                data_[i].push_back(std::stoi(sep_num[j]));
            }
        }
    } 

private:
    std::vector<std::vector<Element>> data_;
};


// TODO: move to validator

class TestValidator {
public:
    static bool Validate(const Validatable *answer, const Solvable *question) {
        return (CheckChanged(answer, question) && CheckConstraints(answer));
    }

private:
    static bool CheckChanged(const Validatable *answer, const Solvable *question) {
        for (size_t i = 0; i < SIZE; ++i) {
            for (size_t j = 0; j < SIZE; ++j) {
                if (question->GetElement(i, j) != UNFILLED) {
                    if (question->GetElement(i, j) != answer->GetElement(i, j))
                        return false;
                }
            }
        }
        return true;
    }

    static bool CheckConstraints(const Validatable *answer) {
        return CheckRows(answer) && CheckColumns(answer) && CheckSubblocks(answer);
    }

    static bool CheckRows(const Validatable *answer) {
        for (size_t i = 0; i < SIZE; ++i) {
            bool exist[N_NUM];
            for (size_t j = 0; j < SIZE; ++j) {
                exist[j] = false;
            }

            for (size_t j = 0; j < SIZE; ++j) {
                Element ele = answer->GetElement(i, j);
                SUDOKU_ASSERT(ele > UNFILLED);
                SUDOKU_ASSERT(ele < N_NUM);
                exist[j] = ele;
            }

            for (size_t j = 1; j < SIZE; ++j) {
                if(exist[j] == false)
                    return false;
            }
        }
        return true;
    }

    static bool CheckColumns(const Validatable *answer) {
        for (size_t i = 0; i < SIZE; ++i) {
            bool exist[N_NUM];
            for (size_t j = 0; j < SIZE; ++j) {
                exist[j] = false;
            }

            for (size_t j = 0; j < SIZE; ++j) {
                Element ele = answer->GetElement(i, j);
                SUDOKU_ASSERT(ele > UNFILLED);
                SUDOKU_ASSERT(ele < N_NUM);
                exist[j] = ele;
            }

            for (size_t j = 1; j < SIZE; ++j) {
                if(exist[j] == false)
                    return false;
            }
        }

        return true;

    }

    static bool CheckSubblocks(const Validatable *answer) {
        for (size_t i = 0; i < N_BLOCKS_1D; ++i) {
            for (size_t j = 0; j < N_BLOCKS_1D; ++j) {
                if (!CheckOneBlock(answer, i, j))
                    return false;
            }
        }

        return true;
    }

    static bool CheckOneBlock(const Validatable *answer, size_t blk_y_idx, size_t blk_x_idx) {
        size_t x_start = blk_x_idx * SUB_SIZE;
        size_t y_start = blk_y_idx * SUB_SIZE;
        bool exist[N_NUM];
        for (size_t i = 0; i < N_NUM; ++i)
            exist[i] = false;
        
        for (size_t i = x_start; i < x_start + SUB_SIZE; ++i) {
            for (size_t j = y_start; j < y_start + SUB_SIZE; ++j) {
                Element ele = answer->GetElement(i, j);
                SUDOKU_ASSERT(ele > UNFILLED);
                SUDOKU_ASSERT(ele < N_NUM);
                exist[ele] = true;
            }
        }
        for (size_t i = 1; i < N_NUM; ++i) {
            if (!exist[i])
                return false;
        }
        return true;
    }
};


void TestProblemStateSolved() {
    const std::string in_csv = "test-data-1.csv";
    TestSudoku ts;
    ts.ReadFromCSV(in_csv);
    Timer tm;
    ProblemStateBase ps(&ts);
    std::cout << "Take " << tm.Elapsed() << " to set up ProblemState from a solved sudoku" << std::endl;
    assert(ps.CheckSolved());
}

void TestProblemStateFixedByConstrant() {
    const std::string in_csv = "test-data-2.csv";
    TestSudoku ts;
    ts.ReadFromCSV(in_csv);
    ProblemStateBase ps(&ts);
    size_t x_idx, y_idx;
    size_t n_pos = ps.GetIdxWithMinPossibility(x_idx, y_idx);
    assert(n_pos == 1);
    assert(x_idx == 0);
    assert(y_idx == 0);
}

void TestProblemStateFixedByPeers() {
    const std::string in_csv = "test-data-3.csv";
    TestSudoku ts;
    ts.ReadFromCSV(in_csv);
    ProblemStateBase ps(&ts);
    size_t x_idx, y_idx;
    Element val;
    bool ret = ps.GetIdxFixedByPeers(x_idx, y_idx, val);
    assert(ret);
    assert(val == 3);
    assert(x_idx == 0);
    assert(y_idx == 0); 
}

void TestProblemStateCopyConstructer() {
    const std::string in_csv = "test-data-4.csv";
    TestSudoku ts;
    ts.ReadFromCSV(in_csv);
    ProblemStateBase ps(&ts);
    Timer tm;
    ProblemStateBase ps_copy(ps);
    std::cout << "Take " << tm.Elapsed() << " to finish copy constructor" << std::endl;
    assert(!ps_copy.CheckSolved());
    ps_copy.Set(8, 7, 7);
    ps_copy.Set(8, 8, 5);
    assert(ps_copy.CheckSolved());
    assert(!ps.CheckSolved());
}

void TestProblemStateAssignOps() {
    const std::string in_csv = "test-data-5.csv";
    TestSudoku ts;
    ts.ReadFromCSV(in_csv);
    ProblemStateBase ps(&ts);

    ProblemStateBase ps2 = ps;
    size_t x_idx, y_idx;
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

void TestSerialSolver() {
    const std::string in_csv = "test-data-6.csv";
    TestSudoku ts;
    ts.ReadFromCSV(in_csv);
    SolverSerial ss;
    SudokuAnswer answer;
    
    Timer tm;
    bool ret = ss.Solve(ts, answer);
    std::cout << "Take " << tm.Elapsed() << " to solve" << std::endl;

    assert(ret);

    TestValidator val;
    assert(val.Validate(&answer, &ts));
}


void TestSerialSolver2() {
    const std::string in_csv = "test-data-7.csv";
    TestSudoku ts;
    ts.ReadFromCSV(in_csv);
    SolverSerial ss;
    SudokuAnswer answer;
    
    Timer tm;
    bool ret = ss.Solve(ts, answer);
    std::cout << "Take " << tm.Elapsed() << " to solve" << std::endl;

    assert(ret);

    TestValidator val;
    assert(val.Validate(&answer, &ts));
}

}

int main() {
    using namespace sudoku;
    // TestProblemStateSolved();
    // TestProblemStateFixedByConstrant();
    // TestProblemStateFixedByPeers();
    // TestProblemStateCopyConstructer();
    // TestProblemStateAssignOps();
    TestSerialSolver();
 
    TestSerialSolver2();
    return 0;
}
