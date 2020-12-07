// a class for generating sudoku (Yi Liu)

#ifndef SUDOKU_GENERATOR_GENERATOR_H_
#define SUDOKU_GENERATOR_GENERATOR_H_


#include <iostream>

#include "generator/sudoku.h"

namespace sudoku {


class Generator {
public:
    Generator();
    Generator(Element *seed);
    void CreateSudoku(int difficulty, Sudoku &ret);

private:
    void CreateRandomSudoku();
    void Rotate();
    void Flip();
    void FlipByThreeRow();
    void FlipByThreeCol();

    Element seed_[SIZE][SIZE];
    Element new_arr_[SIZE][SIZE];  // buffer for permutation
    DISALLOW_CLASS_COPY_AND_ASSIGN(Generator);
};


}

#endif
