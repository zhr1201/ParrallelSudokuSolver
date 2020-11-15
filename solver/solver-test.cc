#include <vector>
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
    // can't pass need to be fixed
    // const std::string in_csv = "test-data-1.csv";
    // TestSudoku ts;
    // ts.ReadFromCSV(in_csv);
    // ProblemStateBase ps(&ts);
    // assert(ps.CheckValid());
}

void TestProblemFixedByConstrant() {

}

void TestProblemFixedByPeers() {

}

}

int main() {
    using namespace sudoku;
    TestProblemStateSolved();
    TestProblemFixedByConstrant();
    TestProblemFixedByPeers();
    return 0;
}
