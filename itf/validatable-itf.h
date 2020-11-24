#ifndef SUDOKU_ITF_VALIDATABLE_ITF_H_
#define SUDOKU_ITF_VALIDATABLE_ITF_H_

#include <util/global.h>

namespace sudoku {


class Validatable {
public:
    virtual Element GetElement(size_t x_idx, size_t y_idx) const = 0;
    virtual ~Validatable() {};
};


}

#endif