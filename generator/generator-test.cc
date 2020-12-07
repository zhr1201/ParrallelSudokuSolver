#include <iostream>

#include "generator/generator.h"
#include "generator/file-parser.h"


namespace sudoku {

void TestGenerateSudoku() {
    Generator gen;
    Sudoku ret;
    gen.CreateSudoku(20, ret);

    // can't test it, just output it and check;
    for (uint i = 0; i < SIZE; ++i) {
        for (uint j = 0; j < SIZE; ++j) {
            std::cout << ret.GetElement(i, j) << ',';
        }
        std::cout << std::endl;
    }
}

void TestOutput() {
    
    int problem_num = 10;
    Generator gen;
    std::vector<Solvable*> sudoku_arr;
    for (uint_t i = 0; i < problem_num; ++i) {
        Sudoku *sudoku = new Sudoku();
        gen.CreateSudoku(10, *sudoku);
        sudoku_arr.push_back((Solvable*)sudoku);
    }

    FileParser fp;
    // check it manually
    fp.WriteToFile("testout.csv", &sudoku_arr);
    std::vector<Solvable*>::iterator iter = sudoku_arr.begin();
    for (; iter != sudoku_arr.end(); ++iter) {
        free(*iter);
    }

}

void TestIOConsitency() {
    int problem_num = 10;
    Generator gen;
    std::vector<Solvable*> sudoku_arr;
    for (uint_t i = 0; i < problem_num; ++i) {
        Sudoku *sudoku = new Sudoku();
        gen.CreateSudoku(20, *sudoku);
        sudoku_arr.push_back((Solvable*)sudoku);
    }

    std::string str = "testout.csv";
    FileParser fp;
    // check it manually
    fp.WriteToFile(str, &sudoku_arr);

    std::vector<Solvable*> *read_vec = nullptr;
    fp.ReadFromFile(str, read_vec);

    for (uint_t i = 0; i < problem_num; ++i) {
        Solvable *in_sudoku = sudoku_arr[i];
        Solvable *out_sudoku = (*read_vec)[i];
        for (uint_t j = 0; j < SIZE; ++j) {
            for (uint_t k = 0; k < SIZE; ++k) {
                assert(in_sudoku->GetElement(j, k) == out_sudoku->GetElement(j, k));
            }
        }
    }

    // std::vector<Solvable*>::iterator iter = sudoku_arr.begin();
    // for (; iter != sudoku_arr.end(); ++iter) {
    //     free(*iter);
    // }

    // iter = read_vec->begin();
    // for (; iter != read_vec->end(); ++iter) {
    //     free(*iter);
    // }
}

}

int main() {
    using namespace sudoku;
    TestGenerateSudoku();
    TestOutput();
    TestIOConsitency();
}
