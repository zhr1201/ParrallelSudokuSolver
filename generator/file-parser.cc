// Read .csv file and save the data as Sudoku Array


#include <vector>
#include <fstream>
#include <sstream>
#include <string>

#include "itf/solvable-itf.h"
#include "generator/file-parser.h"
#include "util/string-utils.h"


namespace sudoku {


void FileParser::WriteToFile(std::string out_file, std::vector<Solvable*> *in_vec) {
    std::ofstream o_fs(out_file);
    solvable_iter iter = in_vec->begin();
    int count = 0;
    for (; iter != in_vec->end(); ++iter) {
        WriteOne(*iter, o_fs);
        ++count;
    }
}

void FileParser::WriteToFile(std::string out_file, std::vector<Validatable*> *in_vec) {
    std::ofstream o_fs(out_file);
    validatable_iter iter = in_vec->begin();
    int count = 0;
    for (; iter != in_vec->end(); ++iter) {
        WriteOne(*iter, o_fs);
        ++count;
    }
}

void FileParser::ReadFromFile(std::string input_file, std::vector<Solvable*> *&ret) {

    //read input file
    ret = new std::vector<Solvable*>();
    std::ifstream i_fs(input_file);
    Sudoku *tmp = nullptr;
    int count = 0;
    while(ReadOne(tmp, i_fs)) {
        ret->push_back(tmp);
        count++;
    }
}

void FileParser::WriteOne(Solvable *data, std::ofstream &file) {
    file << std::endl;
    for (uint_t i = 0; i < SIZE; ++i) {
        for (uint_t j = 0; j < SIZE; ++j) {
            file << data->GetElement(j, i) << ',';
        }
        file << std::endl;
    }
}


void FileParser::WriteOne(Validatable *data, std::ofstream &file) {
    file << std::endl;
    for (uint_t i = 0; i < SIZE; ++i) {
        for (uint_t j = 0; j < SIZE; ++j) {
            file << data->GetElement(j, i) << ',';
        }
        file << std::endl;
    }
}

bool FileParser::ReadOne(Sudoku *&data, std::ifstream &file) {
    std::string line;
    if (!std::getline(file, line))
        return false;

    data = new Sudoku();
    for (uint_t i = 0; i < SIZE; ++i) {
        std::getline(file, line);
        std::vector<std::string> sep_num = split(line, ",");
        for (uint_t j = 0; j < SIZE; ++j) {
            data->data_[i][j] = stoi(sep_num[j]);
        }
    }
    return true;
}

}
