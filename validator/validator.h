// simple validator for validation (Haoran Zhou)

#ifndef SUDOKU_EVAL_VALIDATOR_H_
#define SUDOKU_EVAL_VALIDATOR_H_

#include "util/global.h"
#include "itf/validatable-itf.h"
#include "itf/solvable-itf.h"


namespace sudoku {

class Validator {
public:
    static bool Validate(const Validatable *answer, const Solvable *question) {
        return (CheckChanged(answer, question) && CheckConstraints(answer));
    }

    static bool ValidateBatch(const std::vector<Validatable*> answers, const std::vector <Solvable*> questions);

private:
    static bool CheckChanged(const Validatable *answer, const Solvable *question); 

    static bool CheckConstraints(const Validatable *answer) {
        return CheckRows(answer) && CheckColumns(answer) && CheckSubblocks(answer);
    }

    static bool CheckRows(const Validatable *answer);
    static bool CheckColumns(const Validatable *answer);
    static bool CheckSubblocks(const Validatable *answer);
    static bool CheckOneBlock(const Validatable *answer, size_t blk_y_idx, size_t blk_x_idx);

};


}

#endif`