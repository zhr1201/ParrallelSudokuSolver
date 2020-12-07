// A tool for generating multiple random sudoku problems
// Usage: generate-sudoku <number of questions> <difficulty> <output_file>
//

#include <string>
#include <stdlib.h>
#include <stdio.h>

#include "generator/generator.h"
#include "generator/file-parser.h"


int main(int argc, char *argv[]) {
    try {
        using namespace sudoku;
        const char *usage = 
            "Generate sudoku problems and export them to a file. \n"
            "USage: generate-sudoku <number of questions> <difficulty> <output_file>\n";
        
        if (argc != 4) {
            fprintf(stderr, "%s", usage);
            exit(1);
        }

        int n = atoi(argv[1]);
        int difficulty = atoi(argv[2]);
        std::string outfile(argv[3]);

        Generator gen;
        std::vector<Solvable*> sudoku_arr;
        for (uint_t i = 0; i < n; ++i) {
            Sudoku *sudoku = new Sudoku();
            gen.CreateSudoku(difficulty, *sudoku);
            sudoku_arr.push_back((Solvable*)sudoku);
        }

        FileParser fp;
        // check it manually
        fp.WriteToFile(outfile, &sudoku_arr);

        std::vector<Solvable*>::iterator iter = sudoku_arr.begin();
        for (; iter != sudoku_arr.end(); ++iter) {
            free(*iter);
        }
    } catch (const std::exception &e) {
        std::cerr << e.what();
        return -1;
    }
}
