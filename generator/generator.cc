// Generate random Sudoku array

#include "generator.h"
#include <fstream>
#include <iostream>
#include <string>



int sudokuArr[9][9] = {{7, 3, 5, 6, 1, 4, 8, 9, 2},
                       {8, 4, 2, 9, 7, 3, 5, 6, 1},
                       {9, 6, 1, 2, 8, 5, 3, 7, 4},
                       {2, 8, 6, 3, 4, 9, 1, 5, 7},
                       {4, 1, 3, 8, 5, 7, 9, 2, 6},
                       {5, 7, 9, 1, 2, 6, 4, 3, 8},
                       {1, 5, 7, 4, 9, 2, 6, 8, 3},
                       {6, 9, 4, 7, 3, 8, 2, 1, 5},
                       {3, 2, 8, 5, 6, 1, 7, 4, 9}};


//Generate Sudoku Array and write to a .csv file
void generator::createSudoku(int difficulty) {

    //create random Sudoku array by rotate and flip the given array
    createRandomSudoku();

    //Given three difficulty levels
    int elementToBeSolved = difficulty;
    printf("Number of elements to be Filled Out: %d\n", difficulty);

    //replace the number by 0
    while(elementToBeSolved > 0) {
        int col = rand()%9;
        int row = rand()%9;
        if (sudokuArr[col][row]!= 0) {
            sudokuArr[col][row] = 0;
            elementToBeSolved --;
        }
    }


    //write output file
    std::ofstream sudokuFile("sudokuDemo.csv");
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            if (j < 8) {
                sudokuFile << sudokuArr[i][j];
                sudokuFile << ",";
            }
            else if (j == 8) {
                sudokuFile << sudokuArr[i][j];
                sudokuFile << "\r\n";
            }
        }
    }
    sudokuFile.close();

}



//Rotate / flip the array to create new Sudoku array
//Do all of the operations 10 times (with random times of operation each)
void generator::createRandomSudoku() {
    int operTemp = 10;
    while(operTemp > 0) {
        operTemp --;
        int newArr[9][9];
        //rotate
        int rotateCount = rand()%4;
        while(rotateCount > 0) {
            rotateCount --;
            for (int i=0;i<9;i++) {
                for (int j=0;j<9;j++) {
                    newArr[j][8 - i] = sudokuArr[i][j];
                }
            }
            for (int i=0;i<9;i++) {
                for (int j=0;j<9;j++) {
                    sudokuArr[i][j] = newArr[i][j];
                }
            }
        }
        //flip
        int flipCount = rand()%2;
        while(flipCount > 0) {
            flipCount --;
            for (int i=0;i<9;i++) {
                for (int j=0;j<9;j++) {
                    newArr[i][8 - j] = sudokuArr[i][j];
                }
            }
            for (int i=0;i<9;i++) {
                for (int j=0;j<9;j++) {
                    sudokuArr[i][j] = newArr[i][j];
                }
            }
        }
        //flip by three rows / columns
        flipCount = rand()%3;
        while(flipCount > 0) {
            flipCount --;
            for (int i=0;i<3;i++) {
                for (int j=0;j<9;j++) {
                    newArr[(i+1)%3][j] = sudokuArr[i][j];
                }
            }
            for (int i=3;i<6;i++) {
                for (int j=0;j<9;j++) {
                    newArr[(i+1)%3+3][j] = sudokuArr[i][j];
                }
            }
            for (int i=6;i<9;i++) {
                for (int j=0;j<9;j++) {
                    newArr[(i+1)%3+6][j] = sudokuArr[i][j];
                }
            }
            for (int i=0;i<9;i++) {
                for (int j=0;j<9;j++) {
                    sudokuArr[i][j] = newArr[i][j];
                }
            }
        }
        flipCount = rand()%3;
        while(flipCount > 0) {
            flipCount --;
            for (int i=0;i<9;i++) {
                for (int j=0;j<3;j++) {
                    newArr[i][(j+1)%3] = sudokuArr[i][j];
                }
            }
            for (int i=3;i<9;i++) {
                for (int j=3;j<6;j++) {
                    newArr[i][(j+1)%3 + 3] = sudokuArr[i][j];
                }
            }
            for (int i=6;i<9;i++) {
                for (int j=6;j<9;j++) {
                    newArr[i][(j+1)%3 + 6] = sudokuArr[i][j];
                }
            }
            for (int i=0;i<9;i++) {
                for (int j=0;j<9;j++) {
                    sudokuArr[i][j] = newArr[i][j];
                }
            }
        }

    }

}
