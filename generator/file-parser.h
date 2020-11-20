#ifndef FILE_PARSER_H_
#define FILE_PARSER_H_
#include "../itf/solvable-itf.h"

namespace sudoku {

class FileParser : public Solvable {
public:

    virtual Element GetElement(size_t x_idx, size_t y_idx);
    static void readFromCSV(std::string inputFile);

};

}

#endif
