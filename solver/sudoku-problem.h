// A simple implementation of solvable only to be used inside the solver folder (Haoran)

#ifndef SUDOKU_SOLVER_SUDOKU_PROBLEM_H_
#define SUDOKU_SOLVER_SUDOKU_PROBLEM_H_

#include "util/global.h"
#include "util/string-utils.h"
#include "itf/solvable-itf.h"

namespace sudoku {

class SSudoku : public Solvable {
public:

    SSudoku(Element *problem) {
        memcpy(data_, problem, SIZE * SIZE * sizeof(Element));
    }

    SSudoku(const std::string &csv_file) {
        std::ifstream file(csv_file);
        std::string line;
        assert(file.is_open());

        for (uint_t i = 0; i < SIZE; ++i) {

            std::getline(file, line);
            std::vector<std::string> sep_num = split(line, ",");
            for (uint_t j = 0; j < SIZE; ++j) {
                data_[i][j] = stoi(sep_num[j]);
            }
        }
    }

    virtual Element GetElement(uint_t x_idx, uint_t y_idx) const {
        return data_[y_idx][x_idx];
    }

private:
    Element data_[SIZE][SIZE];
};

}


#endif
