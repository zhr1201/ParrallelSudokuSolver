// Read .csv file and save the data as Sudoku Array

#include "../itf/solvable-itf.h"
#include "file-parser.h"
#include "../util/string-utils.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>


namespace sudoku {

int data[9][9];

size_t FileParser::GetElement(size_t x_idx, size_t y_idx) {
    return data[y_idx][x_idx];
}

void FileParser::readFromCSV(std::string inputFile) {
    //read input file
    std::vector< std::vector<std::string> > strData;
    std::string line, field;
    std::vector<std::string> v;
    std::ifstream in(inputFile);
    while (getline(in,line)) {
        v.clear();
        std::stringstream ss(line);
        while (getline(ss,field,',')) {
            v.push_back(field);  // add each field to the 1D array
        }
    strData.push_back(v);  // add the 1D array to the 2D array
    }
    printf("\nThe problem loaded is:\n");
    for (int i=0;i<9;i++) {
        for (int j=0;j<9;j++) {
            std::stringstream k(strData[i][j]);
            k >> data[i][j];
            printf("%d", data[i][j]);
        }
        printf("\n");
    }
}

}
