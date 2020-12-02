#include "generator.h"

int main() {
    printf("Unit Test For Sudoku generator");
    int difficulty = 20;
    std::string fileName = "test1.csv";
    //printf("%d\n", difficulty);
    generator::createSudoku(difficulty, fileName);
    return 0;
}
