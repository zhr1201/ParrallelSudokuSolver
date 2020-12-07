// a class for IO (Yi Liu)

#ifndef SUDOKU_GENERATOR_FILE_PARSER_H_
#define SUDOKU_GENERATOR_FILE_PARSER_H_


#include <iostream>

#include "itf/validatable-itf.h"
#include "generator/sudoku.h"


namespace sudoku {


class FileParser {

    using solvable_iter = std::vector<Solvable*>::iterator;
    using validatable_iter = std::vector<Validatable*>::iterator;

public:
    static void WriteToFile(std::string out_file, std::vector<Solvable*> *in_vec);
    static void WriteToFile(std::string out_file, std::vector<Validatable*> *in_vec);

    // caller responsible for free returned value
    static void ReadFromFile(std::string input_file, std::vector<Solvable*> *&ret);

private:
    static void WriteOne(Solvable *data, std::ofstream &file);
    static void WriteOne(Validatable *data, std::ofstream &file);

    static bool ReadOne(Sudoku *&data, std::ifstream &file);

};


}

#endif
