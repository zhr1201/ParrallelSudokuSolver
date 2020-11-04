#ifndef SOLVABLE_ITF_H
#define SOLVABLE_ITF_H

#include <util/global.h>

namespace sudoku {


class Solvable {
public:
    virtual Element GetElement(size_t x_idx, size_t y_idx) const = 0;
};


}

#endif