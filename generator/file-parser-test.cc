#include "../itf/solvable-itf.h"
#include "file-parser.h"
#include <vector>
#include <string>

int main(int argc, char *argv[]) {
    std::string file_name = argv[1];
    printf("Read the csv file: %s\n", file_name.c_str());
    sudoku::FileParser::readFromCSV(file_name);
    return 0;
}
