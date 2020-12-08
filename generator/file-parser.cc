// Read .csv file and save the data as Sudoku Array


#include <vector>
#include <fstream>
#include <sstream>
#include <string>

#include "itf/solvable-itf.h"
#include "generator/file-parser.h"
#include "util/string-utils.h"


namespace sudoku {


void FileParser::WriteToFile(std::string out_file, Solvable *sudoku) {
    std::ofstream o_fs(out_file);
    for (uint_t i = 0; i < SIZE; ++i) {
        for (uint_t j = 0; j < SIZE; ++j) {
            o_fs << sudoku->GetElement(j, i);
            if (j != SIZE - 1)
                o_fs << ',';
        }
        o_fs << std::endl;
    }
}



void FileParser::ReadFromFile(std::string input_file, Solvable *&ret) {

    //read input file
    Sudoku *tmp = new Sudoku();
    std::ifstream i_fs(input_file);
    std::string line;
    for (uint_t i = 0; i < SIZE; ++i) {
        std::getline(i_fs, line);
        std::vector<std::string> sep_num = split(line, ",");
        for (uint_t j = 0; j < SIZE; ++j) {
            tmp->data_[i][j] = stoi(sep_num[j]);
        }
    }
    ret = tmp;
}

}