// A tool for solve sudoku problems using serial solver
// Usage: solve-sudoku <intput_file (sudoku problems)> <output_file (answers)>
//

#include <string>
#include <stdlib.h>
#include <stdio.h>

#include "solver/solver-serial.h"
#include "util/timer.h"
#include "generator/file-parser.h"


int main(int argc, char *argv[]) {
    try {
        using namespace sudoku;
        const char *usage = 
            "Solve sudoku problems and export them to a file. \n"
            "Usage: solve-sudoku <intput_file (sudoku problems)> <output_file (answers) \n";
        
        if (argc != 3) {
            fprintf(stderr, "%s", usage);
            exit(1);
        }

        std::string infile(argv[1]);
        std::string outfile(argv[2]);

        Solvable *read_sudoku = nullptr;
        SudokuAnswer answer;

        FileParser fp;
        fp.ReadFromFile(infile, read_sudoku);

        SolverSerial *solver = SolverSerial::GetInstance();

        Timer tm;
        solver->Solve(*read_sudoku, answer);
        std::cout << "Took " << tm.Elapsed() << " to solve! \n" << std::endl; 

        AnswerIO ans_writer;
        ans_writer.WriteToFile(outfile, &answer);
        delete read_sudoku;

    } catch (const std::exception &e) {
        std::cerr << e.what();
        return -1;
    }
}
