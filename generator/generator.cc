// Generate random Sudoku array

#include <fstream>
#include <iostream>
#include <string>

#include "generator/generator.h"


namespace sudoku {


Element SUDOKU_SEED[9][9] = {{7, 3, 5, 6, 1, 4, 8, 9, 2},
                             {8, 4, 2, 9, 7, 3, 5, 6, 1},
                             {9, 6, 1, 2, 8, 5, 3, 7, 4},
                             {2, 8, 6, 3, 4, 9, 1, 5, 7},
                             {4, 1, 3, 8, 5, 7, 9, 2, 6},
                             {5, 7, 9, 1, 2, 6, 4, 3, 8},
                             {1, 5, 7, 4, 9, 2, 6, 8, 3},
                             {6, 9, 4, 7, 3, 8, 2, 1, 5},
                             {3, 2, 8, 5, 6, 1, 7, 4, 9}};


Generator::Generator() {
    memcpy(seed_, (Element*)SUDOKU_SEED, sizeof(Element) * SIZE * SIZE);
}

Generator::Generator(Element *seed) {
    memcpy(seed_, seed, sizeof(Element) * SIZE * SIZE);
}


//Generate Sudoku Array and write to a .csv file
void Generator::CreateSudoku(int difficulty, Sudoku &ret) {

    //create random Sudoku array by rotate and flip the given array
    CreateRandomSudoku();
    memcpy(ret.data_, seed_, sizeof(Element) * SIZE * SIZE);
    //replace the number by 0
    int element_to_be_solved = difficulty;

    while (element_to_be_solved > 0) {
        int col = rand() % SIZE;
        int row = rand() % SIZE;
        if (ret.data_[col][row] != UNFILLED) {
            ret.data_[col][row] = UNFILLED;
            element_to_be_solved --;
        }
    }
}


//Rotate / flip the array to create new Sudoku array
//Do all of the operations 10 times (with random times of operation each)
void Generator::CreateRandomSudoku() {
    int oper_temp = NUM_PERMUTE;
    while (oper_temp > 0) {
        oper_temp--;
        SwapHorizontally();
        SwapVertically();
        // flip by three rows / columns
        FlipMain();
        FlipMinor();
    }
}

void Generator::SwapHorizontally() {
    int rotate_count = rand() % 3;
    while (rotate_count > 0) {
        rotate_count--;
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                new_arr_[j][SIZE - 1 - i] = seed_[i][j];
            }
        }
        memcpy(seed_, new_arr_, sizeof(Element) * SIZE * SIZE);
    }
}

void Generator::SwapVertically() {
    int flip_count = rand() % 3;
    while (flip_count > 0) {
        flip_count--;
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                new_arr_[i][SIZE - 1 - j] = seed_[i][j];
            }
        }
        memcpy(seed_, new_arr_, sizeof(Element) * SIZE * SIZE);
    }
}

void Generator::FlipMain() {

    int flip_count = rand() % 3;
    while (flip_count > 0) {
        flip_count--;
        for (uint_t i = 0; i < SIZE; ++i) {
            for (uint_t j = 0; j < SIZE; ++j) {
                new_arr_[i][j] = seed_[j][i];
            }
        }
        memcpy(seed_, new_arr_, sizeof(Element) * SIZE * SIZE);
    }
}

void Generator::FlipMinor() {
    int flip_count = rand() % 3;
    while (flip_count > 0) {
        flip_count--;
        for (uint_t i = 0; i < SIZE; ++i) {
            for (uint_t j = 0; j < SIZE; ++j) {
                new_arr_[i][j] = seed_[SIZE - 1 - j][SIZE - 1 - i]; 
            }
        }
        memcpy(seed_, new_arr_, sizeof(Element) * SIZE * SIZE);
    }
}

}