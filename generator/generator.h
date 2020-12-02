#ifndef GENERATOR_H_
#define GENERATOR_H_
#include <iostream>

class generator {
    public:
        static void createSudoku(int difficulty, std::string fileName);
        static void createRandomSudoku();
};

#endif
