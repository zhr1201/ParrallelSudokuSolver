// sovler/problem-state.c (author: Haoran Zhou)


#include <math.h>
#include "solver/problem-state.h"


namespace sudoku {


inline size_t Idx2Offset(size_t y_idx, size_t x_idx) {
    return y_idx * SIZE + x_idx;
}

inline size_t ProblemStateBase::ElementState::GetIdxInSubList(size_t y_idx, size_t x_idx) {
    return subscriber_idx_[Idx2Offset(y_idx, x_idx)];
}

void ProblemStateBase::InsertIntoList(ElementListNode *&head, ElementListNode *&tail, ElementListNode *insert) {
    if (head == nullptr) {
        head = insert;
        tail = insert;
        return;
    }

    tail->next_ = insert;
    insert->prev_ = tail;
    tail = tail->next_;
}

void ProblemStateBase::RemoveFromList(ElementListNode *&head, ElementListNode *&tail, ElementListNode *remove) {
    ElementListNode *prev = remove->prev_;
    ElementListNode *next = remove->next_;

    if (prev != nullptr && next != nullptr) {
        prev->next_ = next;
        next->prev_ = prev;
        return;
    }

    if (prev == nullptr) {
        SUDOKU_ASSERT(remove == head);
        head = next;
        head->prev_ = nullptr;
    }

    if (next == nullptr) {
        SUDOKU_ASSERT(remove == tail);
        tail = prev;
        tail->next_ = nullptr;
    }

}


void ProblemStateBase::ElementState::ConstructSubscriberIdx() {
    size_t blk_y_start = y_idx_ / SUB_SIZE * SUB_SIZE;
    size_t blk_x_start = x_idx_ / SUB_SIZE * SUB_SIZE;
    
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
    ElementListNode *tmp = &subscriber_list_[idx];
    SUDOKU_ASSERT(tmp->state_ == nullptr);
    tmp->state_ = state;
    InsertIntoList(head_, tail_, tmp);
}

void ProblemStateBase::ElementState::UnSubscribe(ElementState *state) {
    size_t idx = GetIdxInSubList(state->y_idx_, state->x_idx_);
    std::cout << "Unsub " << state->y_idx_ << " " << state->x_idx_ << " from " << y_idx_ << " " << x_idx_ << std::endl;
    ElementListNode *tmp = &subscriber_list_[idx];
    SUDOKU_ASSERT(tmp != nullptr);
    // RemoveFromList(head_, tail_, tmp);
    // tmp->prev_ = nullptr;
    // tmp->next_ = nullptr;
    // tmp->state_ = nullptr;
    return;
}

void ProblemStateBase::ElementState::UnSubcribeAllForCur() {
    ElementListNode *cur = head_;
    while (cur != nullptr) {
        cur->state_->UnSubscribe(this);
        cur = cur->next_; 
    }
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
    if (!constraints_[val]) {
        SUDOKU_ASSERT(n_possibilities_ != 0);
        if (--n_possibilities_ == 0) {
            return false;
        }
        if (NotifySubscriberPossibilities(val)) {
            return true;
        } else {
            return false;
        }
    } else {
        constraints_[val] = true;
        return true;
    }
}

bool ProblemStateBase::ElementState::UpdatePossibilities(Element val) {
    --peer_possibilities_array_[val];
    SUDOKU_ASSERT(peer_possibilities_array_[val] >= 0);
    if (peer_possibilities_array_[val] == 0) {
        val_fix_ = val;
    }
    return true;
}


// an expensive setup O(N ^ 3), but reduce repeatedly computing active peers later
ProblemStateBase::ProblemStateBase(Solvable *problem) :
        valid_(true) {  
    
    for (size_t i = 0; i < SIZE; ++i) {
        for (size_t j = 0; j < SIZE; ++j) {
            size_t offset = Idx2Offset(i, j);
            ele_list_[offset].state_ = &ele_arr_[offset];
            ele_arr_[offset].y_idx_ = i;
            ele_arr_[offset].x_idx_ = j;
            InsertIntoList(head_, tail_, ele_list_ + offset);
        }
    };

    for (size_t i = 0; i < SIZE; ++i) {
        for (size_t j = 0; j < SIZE; ++j) {
            SubscribePeers(i, j);
        }
    }

    for (size_t i = 0; i < SIZE; ++i) {
        for (size_t j = 0; j < SIZE; ++j) {
            Element tmp = problem->GetElement(j, i);
            if (tmp != UNFILLED) {
                if (!Set(i, j, tmp)) {
                    valid_ = false;
                }
            }
        }
    }        
}

void ProblemStateBase::SubscribePeers(size_t y_idx, size_t x_idx) {
    ElementState *cur = &ele_arr_[Idx2Offset(y_idx, x_idx)];
    size_t blk_y_start = y_idx / SUB_SIZE * SUB_SIZE;
    size_t blk_x_start = x_idx / SUB_SIZE * SUB_SIZE;
    
    size_t idx_counter = 0;

    for (size_t i = 0; i < SIZE; ++i) {
        if (i != x_idx) {
            cur->subscriber_idx_[Idx2Offset(y_idx, i)] = idx_counter++;
            cur->Subscribe(&ele_arr_[Idx2Offset(y_idx, i)]);
        }
    }

    for (size_t i = 0; i < SIZE; ++i) {
        if (i != y_idx) {
            cur->subscriber_idx_[Idx2Offset(i, x_idx)] = idx_counter++;
            cur->Subscribe(&ele_arr_[Idx2Offset(i, x_idx)]);
        }
    }

    for (size_t i = blk_y_start; i < blk_y_start + SUB_SIZE; ++i) {
        for (size_t j = blk_x_start; j < blk_x_start + SUB_SIZE; ++j) {
            if (i != y_idx && j != x_idx) {
                cur->subscriber_idx_[Idx2Offset(i, j)] = idx_counter++;
                cur->Subscribe(&ele_arr_[Idx2Offset(i, j)]);
            }
        }
    }
}

bool ProblemStateBase::Set(size_t y_idx, size_t x_idx, Element val) {
    std::cout << y_idx << " " << x_idx << " " << val << std::endl;
    ElementState node = ele_arr_[Idx2Offset(y_idx, x_idx)];
    SUDOKU_ASSERT(node.val_ == UNFILLED);

    if (node.constraints_[val]) {
        std::cout << "Constaint violation" << std::endl;
        return false;
    }
    if (y_idx == 8 && x_idx == 5) {
        std::cout << "Break" << std::endl;
    }

    node.val_ = val;
    node.UnSubcribeAllForCur();
    std::cout << "Remove from active list" << std::endl;
    RemoveFromList(head_, tail_, &ele_list_[Idx2Offset(y_idx, x_idx)]);
    return node.NotifySubscriberConstraints();
}

size_t ProblemStateBase::GetIdxWithMinPossibility(size_t &y_idx, size_t &x_idx) const {
    ElementListNode *cur = head_;
    ElementListNode *min = head_;
    while (cur != nullptr) {

        min = (cur->state_->n_possibilities_ < min->state_->n_possibilities_ ? cur : min);
        cur = cur->next_;
    }
    if (min) {
        x_idx = min->state_->x_idx_;
        y_idx = min->state_->y_idx_;
        return min->state_->n_possibilities_;
    }
    else {
        return 0;
    }
    
}

bool ProblemStateBase::GetIdxFixedByPeers(size_t &y_idx, size_t &x_idx, Element &val) const {
    ElementListNode *cur = head_;
    while (cur != nullptr) {
        if (cur->state_->val_fix_ != UNFILLED) {
            y_idx = cur->state_->y_idx_;
            x_idx = cur->state_->x_idx_;
            val = cur->state_->val_fix_;
            return true;
        }
    }
    return false;
}

}