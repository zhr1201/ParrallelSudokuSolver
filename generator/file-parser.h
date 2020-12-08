// a class for IO (Yi Liu)

#ifndef SUDOKU_GENERATOR_FILE_PARSER_H_
#define SUDOKU_GENERATOR_FILE_PARSER_H_


#include <iostream>

#include "itf/validatable-itf.h"
#include "generator/sudoku.h"


namespace sudoku {


class FileParser {


public:
    static void WriteToFile(std::string out_file, Solvable *sudoku);

    // caller responsible for free returned value
    static void ReadFromFile(std::string input_file, Solvable *&ret);

};


}

#endif
