#include "generator.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

int main(int argc, char *argv[]) {
    generator::createSudoku();
}

void generator::createSudoku() {
    //Manually given a Sudoku array
    int sudokuArr[9][9] = {{7,3,5,6,1,4,8,9,2},
                           {8,4,2,9,7,3,5,6,1},
                           (9,6,1,2,8,5,3,7,4),
                           {2,8,6,3,4,9,1,5,7},
                           {4,1,3,8,5,7,9,2,6},
                           {5,7,9,1,2,6,4,3,8},
                           {1,5,7,4,9,2,6,8,3},
                           {6,9,4,7,3,8,2,1,5},
                           {3,2,8,5,6,1,7,4,9}};

    //Given three difficulty levels
    //TODO: This will be replaced by an input argument
    int difficulty = 0;
    int itemToBeSolved;
    switch(difficulty) {
        case 0 :
            itemToBeSolved = 20;
            break;
        case 1:
            itemToBeSolved = 30;
            break;
        case 2:
            itemToBeSolved = 40;
            break;
        default:
            itemToBeSolved = 20;
            break;
    }

    //replace the number by 0
    while(itemToBeSolved > 0) {
        int col = rand()%9;
        int row = rand()%9;
        if (sudokuArr[col][row]!= 0) {
            sudokuArr[col][row] = 0;
            itemToBeSolved --;
        }
    }


    //write output file
    std::string problemFile = "sudokuDemo.csv";
    std::ofstream problemfile;
    problemfile.open(problemFile);
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            std::ostringstream out;
            out << sudokuArr[i][j];
            if (j < 8) {
                problemfile << out.str() << ",";
            }
            else if (j == 8) {
                problemfile << out.str() << "\r\n";
            }
        }
    }


}
