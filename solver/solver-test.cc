#include <vector>
#include "util/timer.h"
#include "util/global.h"
#include "solver/problem-state.h"
#include "util/string-utils.h"


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

void TestProblemStateSolved() {
    const std::string in_csv = "test-data-1.csv";
    TestSudoku ts;
    ts.ReadFromCSV(in_csv);
    Timer tm;
    ProblemStateBase ps(&ts);
    std::cout << "Takes " << tm.Elapsed() << " to set up a solved sudoku" << std::endl;
    assert(ps.CheckValid());
}

void TestProblemFixedByConstrant() {
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

void TestProblemFixedByPeers() {
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

}

int main() {
    using namespace sudoku;
    TestProblemStateSolved();
    TestProblemFixedByConstrant();
    TestProblemFixedByPeers();
    return 0;
}
