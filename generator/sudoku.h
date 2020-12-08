#ifndef SUDOKU_GENERATOR_SUDOKU_H_
#define SUDOKU_GENERATOR_SUDOKU_H_

#include "util/global.h"
#include "util/string-utils.h"
#include "itf/solvable-itf.h"

namespace sudoku {


class Generator;


class Sudoku : public Solvable {
public:
    virtual Element GetElement(uint_t x_idx, uint_t y_idx) const {
        return data_[y_idx][x_idx];
    }

private:
    Element data_[SIZE][SIZE];
    friend class Generator;
    friend class FileParser;
};

}


#endif