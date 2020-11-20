#include "generator.h"

int main(int argc, char *argv[])
{
    int difficulty = atoi(argv[1]);
    printf("%d\n", difficulty);
    generator::createSudoku(difficulty);
    return 0;
}
