// A tool for validate sudoku answers
// Usage: validate <problem file> <answer file>
//

#include <string>
#include <stdlib.h>
#include <stdio.h>

#include "validator/validator.h"
#include "generator/file-parser.h"
#include "solver/solver.h"


int main(int argc, char *argv[]) {
    try {
        using namespace sudoku;
        const char *usage = 
            "A tool for validate sudoku answers \n"
            "  Usage: validate <problem file> <answer file> \n";
        if (argc != 3) {
            fprintf(stderr, "%s", usage);
            exit(1);
        }

        std::string problem(argv[1]);
        std::string answer(argv[2]);

        Solvable *read_sudoku = nullptr;
        Validatable *solved_sudoku = nullptr;

        FileParser fp;
        fp.ReadFromFile(problem, read_sudoku);

        AnswerIO as_reader;
        as_reader.ReadFromFile(answer, solved_sudoku);
        
        Validator val;
        bool ret = val.Validate(solved_sudoku, read_sudoku);
        std::cout << "Validator returned " << ret << std::endl;
        delete read_sudoku;
        delete solved_sudoku;

    } catch (const std::exception &e) {
        std::cerr << e.what();
        return -1;
    }
}
