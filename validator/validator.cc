#include "validator/validator.h"


namespace sudoku {

bool Validator::ValidateBatch(const std::vector<Validatable*> answers, const std::vector <Solvable*> questions) {
    if (answers.size() != questions.size())
        return false;
    for (size_t i = 0; i < answers.size(); ++i) {
        if (!Validate(answers[i], questions[i]))
            return false;
    }
    return true;
}

bool Validator::CheckChanged(const Validatable *answer, const Solvable *question) {
    for (size_t i = 0; i < SIZE; ++i) {
        for (size_t j = 0; j < SIZE; ++j) {
            if (question->GetElement(i, j) != UNFILLED) {
                if (question->GetElement(i, j) != answer->GetElement(i, j))
                    return false;
            }
        }
    }
    return true;
}

bool Validator::CheckRows(const Validatable *answer) {
    for (size_t i = 0; i < SIZE; ++i) {
        bool exist[N_NUM];
        for (size_t j = 0; j < SIZE; ++j) {
            exist[j] = false;
        }

        for (size_t j = 0; j < SIZE; ++j) {
            Element ele = answer->GetElement(i, j);
            SUDOKU_ASSERT(ele > UNFILLED);
            SUDOKU_ASSERT(ele < N_NUM);
            exist[j] = ele;
        }

        for (size_t j = 1; j < SIZE; ++j) {
            if(exist[j] == false)
                return false;
        }
    }
    return true;
}

bool Validator::CheckColumns(const Validatable *answer) {
    for (size_t i = 0; i < SIZE; ++i) {
        bool exist[N_NUM];
        for (size_t j = 0; j < SIZE; ++j) {
            exist[j] = false;
        }

        for (size_t j = 0; j < SIZE; ++j) {
            Element ele = answer->GetElement(i, j);
            SUDOKU_ASSERT(ele > UNFILLED);
            SUDOKU_ASSERT(ele < N_NUM);
            exist[j] = ele;
        }

        for (size_t j = 1; j < SIZE; ++j) {
            if(exist[j] == false)
                return false;
        }
    }

    return true;
}

bool Validator::CheckSubblocks(const Validatable *answer) {
    for (size_t i = 0; i < N_BLOCKS_1D; ++i) {
        for (size_t j = 0; j < N_BLOCKS_1D; ++j) {
            if (!CheckOneBlock(answer, i, j))
                return false;
        }
    }

    return true;
}


bool Validator::CheckOneBlock(const Validatable *answer, size_t blk_y_idx, size_t blk_x_idx) {
    size_t x_start = blk_x_idx * SUB_SIZE;
    size_t y_start = blk_y_idx * SUB_SIZE;
    bool exist[N_NUM];
    for (size_t i = 0; i < N_NUM; ++i)
        exist[i] = false;
    
    for (size_t i = x_start; i < x_start + SUB_SIZE; ++i) {
        for (size_t j = y_start; j < y_start + SUB_SIZE; ++j) {
            Element ele = answer->GetElement(i, j);
            SUDOKU_ASSERT(ele > UNFILLED);
            SUDOKU_ASSERT(ele < N_NUM);
            exist[ele] = true;
        }
    }
    for (size_t i = 1; i < N_NUM; ++i) {
        if (!exist[i])
            return false;
    }
    return true;
}



}
