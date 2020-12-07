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
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            ret.data_[i][j] = SUDOKU_SEED[i][j];
        }
    }
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
        //rotate
        Rotate();
        // flip
        Flip();
        // flip by three rows / columns
        FlipByThreeRow();
        FlipByThreeCol();
    }
}

void Generator::Rotate() {
    int rotate_count = rand() % 4;
    while (rotate_count > 0) {
        rotate_count--;
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                new_arr_[j][8 - i] = seed_[i][j];
            }
        }
        memcpy(seed_, new_arr_, sizeof(Element) * SIZE * SIZE);
    }
}

void Generator::Flip() {
    int flip_count = rand() % 2;
    while (flip_count > 0) {
        flip_count--;
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                new_arr_[i][8 - j] = seed_[i][j];
            }
        }
        memcpy(seed_, new_arr_, sizeof(Element) * SIZE * SIZE);
    }
}

void Generator::FlipByThreeRow() {

    int flip_count = rand() % 3;
    while (flip_count > 0) {
        flip_count --;
        for (int i = 0;i < 3; i++) {
            for (int j = 0; j < 9; j++) {
                new_arr_[(i + 1) % 3][j] = seed_[i][j];
            }
        }
        for (int i = 3; i < 6;i++) {
            for (int j = 0; j < 9; j++) {
                new_arr_[(i + 1) % 3 + 3][j] = seed_[i][j];
            }
        }
        for (int i = 6; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                new_arr_[(i + 1) % 3 + 6][j] = seed_[i][j];
            }
        }
        memcpy(seed_, new_arr_, sizeof(Element) * SIZE * SIZE);

    }
}

void Generator::FlipBThreeCol() {
    int flip_count = rand() % 3;
    while (flip_count > 0) {
        flip_count --;
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 3; j++) {
                new_arr_[i][(j + 1) % 3] = seed_[i][j];
            }
        }
        for (int i = 3; i < 9; i++) {
            for (int j = 3; j < 6; j++) {
                new_arr_[i][(j + 1) % 3 + 3] = seed_[i][j];
            }
        }
        for (int i = 6; i < 9; i++) {
            for (int j = 6; j < 9; j++) {
                new_arr_[i][(j + 1) % 3 + 6] = seed_[i][j];
            }
        }
        memcpy(seed_, new_arr_, sizeof(Element) * SIZE * SIZE);
    }
}

}