#include <math.h>
#include "solver/problem-state.h"


namespace sudoku {


inline size_t Idx2Offset(size_t y_idx, size_t x_idx) {
    return y_idx * SIZE + x_idx;
}

// O(N) but still a little bit expensive
inline size_t ProblemStateBase::ElementState::GetIdxInSubList(size_t y_idx, size_t x_idx) {
    return subscriber_idx_[Idx2Offset(y_idx, x_idx_)];
}


void ProblemStateBase::ElementState::ReconstructSubscriberIdx() {
    size_t blk_y_start = y_idx_ % SUB_SIZE * SUB_SIZE;
    size_t blk_x_start = x_idx_ % SUB_SIZE * SUB_SIZE;
    
    size_t idx_counter = 0;

    for (size_t i = 0; i < SIZE; ++i) {
        if (i != x_idx_) {
            subscriber_idx_[Idx2Offset(y_idx_, i)] = idx_counter++;
        }
    }

    for (size_t i = 0; i < SIZE; ++i) {
        if (i != y_idx_) {
            subscriber_idx_[Idx2Offset(i, x_idx_)] = idx_counter++;
        }
    }

    for (size_t i = blk_y_start; i < blk_y_start + SUB_SIZE; ++i) {
        for (size_t j = blk_x_start; j < blk_x_start + SUB_SIZE; ++j) {
            if (i != y_idx_ && j != x_idx_) {
                subscriber_idx_[Idx2Offset(i, j)] = idx_counter++;
            }
        }
    }
}

void ProblemStateBase::ElementState::Subscribe(ElementState* state) {
    size_t idx = GetIdxInSubList(state->y_idx_, state->x_idx_);
    SUDOKU_ASSERT(subscriber_list_[idx].state_ == nullptr);
    subscriber_list_[idx].state_ = state;
    if (head_ == nullptr) {
        head_ = &subscriber_list_[idx];
        tail_ = &subscriber_list_[idx];
        return;
    }

    tail_->next_ = &subscriber_list_[idx];
    subscriber_list_[idx].prev_ = tail_;
    tail_ = tail_->next_;
}

void ProblemStateBase::ElementState::UnSubScribe(ElementState *state) {
    size_t idx = GetIdxInSubList(state->y_idx_, state->x_idx_);
    ElementListNode* cur = &subscriber_list_[idx];
    SUDOKU_ASSERT(cur != nullptr);
    ElementListNode* prev = cur->prev_;
    ElementListNode* next = cur->next_;
    cur->prev_ = nullptr;
    cur->next_ = nullptr;
    cur->state_ = nullptr;

    if (prev == nullptr) {
        SUDOKU_ASSERT(cur == head_);
        head_ = next;
        return;
    }

    if (next == nullptr) {
        SUDOKU_ASSERT(cur == tail_);
        tail_ = prev;
        return;
    }

    prev->next_ = next;
    next->prev_ = prev;
    return;
}

bool ProblemStateBase::ElementState::NotifySubscriberConstraints() {
    Element constraint = val_;
    ElementListNode *cur = head_;
    while (cur != nullptr) {
        if (!cur->state_->UpdateConstraints(constraint)) {
            return false;
        }
        cur = cur->next_;
    }
    return true;
}

bool ProblemStateBase::ElementState::NotifySubscriberPossibilities(Element val) {
    ElementListNode *cur = head_;
    while (cur != nullptr) {
        if (!cur->state_->UpdatePossibilities(val)) {
            return false;
        }
        cur = cur->next_;
    }
    return true;
}

bool ProblemStateBase::ElementState::UpdateConstraints(Element val) {
    if (!constraints_[val])
        --n_possibilities_;
    constraints_[val] = true;
    return true;
}

bool ProblemStateBase::ElementState::UpdatePossibilities(Element val) {
    if ((--peer_possibilities_array_[val]) > 0)
        return true;
    else
        return false;
    
}


// an expensive setup O(N^3), but reduce repeatedly computing active peers later
ProblemStateBase::ProblemStateBase(Solvable *problem) {  
    for (size_t i = 0; i < SIZE; ++i) {
        for (size_t j = 0; j < SIZE; ++j) {
            SubscribePeers(i, j);
        }
    }        
}


void ProblemStateBase::SubscribePeers(size_t y_idx, size_t x_idx) {
    ElementState *cur = &ele_arr_[Idx2Offset(y_idx, x_idx)];
    size_t blk_y_start = y_idx % SUB_SIZE * SUB_SIZE;
    size_t blk_x_start = x_idx % SUB_SIZE * SUB_SIZE;
    
    size_t idx_counter = 0;

    for (size_t i = 0; i < blk_y_start; ++i) {
        if (i != x_idx) {
            cur->Subscribe(&ele_arr_[Idx2Offset(y_idx, i)]);
            cur->subscriber_idx_[Idx2Offset(y_idx, i)] = idx_counter++;
        }
    }

    for (size_t i = 0; i < blk_y_start; ++i) {
        if (i != y_idx) {
            cur->Subscribe(&ele_arr_[Idx2Offset(i, x_idx)]);
            cur->subscriber_idx_[Idx2Offset(i, x_idx)] = idx_counter++;
        }
    }

    for (size_t i = blk_y_start; i < blk_y_start + SUB_SIZE; ++i) {
        for (size_t j = blk_x_start; j < blk_x_start + SUB_SIZE; ++j) {
            if (i != y_idx && j != x_idx) {
                cur->Subscribe(&ele_arr_[Idx2Offset(i, j)]);
                cur->subscriber_idx_[Idx2Offset(i, j)] = idx_counter++;
            }
        }
    }
}




}