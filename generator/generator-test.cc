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
    
    Generator gen;
    Sudoku *sudoku = new Sudoku();
    gen.CreateSudoku(10, *sudoku);

    FileParser fp;
    // check it manually
    fp.WriteToFile("testout.csv", sudoku);
    delete sudoku;

}

void TestIOConsitency() {
    Generator gen;
    Sudoku *sudoku = new Sudoku();
    gen.CreateSudoku(10, *sudoku);

    std::string str = "testout.csv";
    FileParser fp;
    // check it manually
    fp.WriteToFile(str, sudoku);

    Solvable *read_sudoku = nullptr;
    fp.ReadFromFile(str, read_sudoku);

    Solvable *in_sudoku = sudoku;
    Solvable *out_sudoku = read_sudoku;
    for (uint_t j = 0; j < SIZE; ++j) {
        for (uint_t k = 0; k < SIZE; ++k) {
            assert(in_sudoku->GetElement(j, k) == out_sudoku->GetElement(j, k));
        }
    }
    delete sudoku;
    delete read_sudoku;
}

}

int main() {
    using namespace sudoku;
    TestGenerateSudoku();
    TestOutput();
    TestIOConsitency();
}
