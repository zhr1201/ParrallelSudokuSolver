// A tool for generating multiple random sudoku problems
// Usage: generate-sudoku <difficulty> <output_file>
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
            "USage: generate-sudoku <difficulty> <output_file>\n";
        
        if (argc != 3) {
            fprintf(stderr, "%s", usage);
            exit(1);
        }

        int difficulty = atoi(argv[1]);
        std::string outfile(argv[2]);

        Generator gen;
        
        Sudoku *sudoku = new Sudoku();
        gen.CreateSudoku(difficulty, *sudoku);

        FileParser fp;
        // check it manually
        fp.WriteToFile(outfile, sudoku);

        delete sudoku;
    } catch (const std::exception &e) {
        std::cerr << e.what();
        return -1;
    }
}
