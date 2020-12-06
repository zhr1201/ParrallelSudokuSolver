// sovler/problem-state.c (author: Haoran Zhou)


#include <math.h>
#include "solver/problem-state.h"


namespace sudoku {


inline uint_t ProblemStateBase::ElementState::GetIdxInSubList(uint_t y_idx, uint_t x_idx) {
    return subscriber_idx_[IDX2OFFSET(y_idx, x_idx)];
}



void ProblemStateBase::ElementState::ConstructSubscriberIdx() {
    uint_t blk_y_start = y_idx_ / SUB_SIZE * SUB_SIZE;
    uint_t blk_x_start = x_idx_ / SUB_SIZE * SUB_SIZE;
    
    uint_t idx_counter = 0;

    for (uint_t i = 0; i < SIZE; ++i) {
        if (i != x_idx_) {
            subscriber_idx_[IDX2OFFSET(y_idx_, i)] = idx_counter++;
        }
    }

    for (uint_t i = 0; i < SIZE; ++i) {
        if (i != y_idx_) {
            subscriber_idx_[IDX2OFFSET(i, x_idx_)] = idx_counter++;
        }
    }

    for (uint_t i = blk_y_start; i < blk_y_start + SUB_SIZE; ++i) {
        for (uint_t j = blk_x_start; j < blk_x_start + SUB_SIZE; ++j) {
            if (i != y_idx_ && j != x_idx_) {
                subscriber_idx_[IDX2OFFSET(i, j)] = idx_counter++;
            }
        }
    }
}

void ProblemStateBase::ElementState::Subscribe(ElementState* state) {
    uint_t idx = GetIdxInSubList(state->y_idx_, state->x_idx_);
    ElementListNode *tmp = &subscriber_list_[idx];
    SUDOKU_ASSERT(tmp->state_ == nullptr);
    tmp->state_ = state;
    InsertIntoList(head_, tail_, tmp);
}

void ProblemStateBase::ElementState::UnSubscribe(ElementState *state) {
    SUDOKU_ASSERT(this != state);
    SUDOKU_ASSERT(this->val_ == UNFILLED);
    uint_t idx = GetIdxInSubList(state->y_idx_, state->x_idx_);
    ElementListNode *tmp = &subscriber_list_[idx];
    SUDOKU_ASSERT(tmp->state_ != nullptr);
    RemoveFromList(head_, tail_, tmp);
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
        constraints_[val] = true;
        SUDOKU_ASSERT(n_possibilities_ > 0);
        if (--n_possibilities_ == 0) {
            return false;
        }
        if (NotifySubscriberPossibilities(val)) {
            return true;
        } else {
            return false;
        }
    } else {
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

void ProblemStateBase::ElementState::SetFromAnother(const ElementState &other, u_longlong_t offset, bool add, u_longlong_t base, u_longlong_t limit) {
    // dangerous but fast

    SUDOKU_ASSERT((offset) == (add ? (u_longlong_t)this - (u_longlong_t)&other : (u_longlong_t)&other - (u_longlong_t)this));
    val_ = other.val_;
    x_idx_ = other.x_idx_;
    y_idx_ = other.y_idx_;
    n_possibilities_ = other.n_possibilities_;
    memcpy(peer_possibilities_array_, other.peer_possibilities_array_, sizeof(uint_t) * N_NUM);
    val_fix_ = other.val_fix_;
    memcpy(constraints_, other.constraints_, sizeof(bool) * N_NUM);

    CopyPointer(head_, other.head_, offset, add, base, limit);
    CopyPointer(tail_, other.tail_, offset, add, base, limit);
    for (uint_t i = 0; i < N_PEERS; ++i) {
        CopyPointer(subscriber_list_[i].state_, other.subscriber_list_[i].state_, offset, add, base, limit);
        CopyPointer(subscriber_list_[i].prev_, other.subscriber_list_[i].prev_, offset, add, base, limit);
        CopyPointer(subscriber_list_[i].next_, other.subscriber_list_[i].next_, offset, add, base, limit);
    }
    memcpy(subscriber_idx_, other.subscriber_idx_, sizeof(uint_t) * N_GRID);
}

bool ProblemStateBase::ElementState::NotifyTaken(Element val) {
    for (uint_t i = 1; i < N_NUM; ++i) {
        if (i != val && !constraints_[i]) {
            if (!NotifySubscriberPossibilities(i))
                return false;
        }
    }
    return true;
}

void ProblemStateBase::ElementState::Clear() {
    val_ = UNFILLED;
    x_idx_ = 0;
    y_idx_ = 0;
    n_possibilities_ = N_NUM - 1;
    std::fill(peer_possibilities_array_, peer_possibilities_array_ + N_NUM, N_PEERS);
    val_fix_ = UNFILLED;
    std::fill(constraints_, constraints_ + N_NUM, false);
    head_ = nullptr;
    tail_ = nullptr;
    for (uint_t i = 0; i < N_PEERS; ++i) {
        subscriber_list_[i].prev_ = nullptr;
        subscriber_list_[i].state_ = nullptr;
        subscriber_list_[i].next_ = nullptr;
    }
    // no need to reset since its going to be the same
    // std::fill(subscriber_idx_, subscriber_idx_ + N_GRID, 0);
}

// an expensive setup O(N ^ 3), but reduce repeatedly computing active peers later
ProblemStateBase::ProblemStateBase(const Solvable *problem) :
         ele_arr_(), valid_(true), ele_list_(), head_(nullptr), tail_(nullptr) {  
    SetProblem(problem);
}


void ProblemStateBase::ResetProblem(const Solvable *problem) {
    Clear();
    SetProblem(problem);
}

void ProblemStateBase::SetProblem(const Solvable *problem) {
    Clear();
    mutex_.Lock();
    for (uint_t i = 0; i < SIZE; ++i) {
        for (uint_t j = 0; j < SIZE; ++j) {
            uint_t offset = IDX2OFFSET(i, j);
            ele_list_[offset].state_ = &ele_arr_[offset];
            ele_arr_[offset].y_idx_ = i;
            ele_arr_[offset].x_idx_ = j;
            InsertIntoList(head_, tail_, ele_list_ + offset);
            ele_arr_[offset].ConstructSubscriberIdx();
        }
    }

    for (uint_t i = 0; i < SIZE; ++i) {
        for (uint_t j = 0; j < SIZE; ++j) {
            mutex_.Unlock();
            SubscribePeers(i, j);
            mutex_.Lock();
        }
    }

    for (uint_t i = 0; i < SIZE; ++i) {
        for (uint_t j = 0; j < SIZE; ++j) {
            Element tmp = problem->GetElement(j, i);
            if (tmp != UNFILLED) {
                mutex_.Unlock();
                bool ret = Set(i, j, tmp);
                mutex_.Lock();
                if (!ret) {
                    valid_ = false;
                }
            }
        }
    }
    mutex_.Unlock();
}

void ProblemStateBase::Clear() {
    Lock lock(mutex_);
    valid_ = true;
    head_ = nullptr;
    tail_ = nullptr;
    for (uint_t i = 0; i < SIZE; ++i) {
        for (uint_t j = 0; j < SIZE; ++j) {
            uint_t offset = IDX2OFFSET(i, j);
            ele_list_[offset].state_ = nullptr;
            ele_list_[offset].prev_ = nullptr; 
            ele_list_[offset].next_ = nullptr;
            ele_arr_[offset].Clear();
        }
    }
}

ProblemStateBase::ProblemStateBase(const ProblemStateBase &other) {
    // adding the offset to pointers
    // extreamly dangeraous but blazingly fast
    SetFromAnother(other);
}

// can't use copy-and-swap here
ProblemStateBase& ProblemStateBase::operator=(const ProblemStateBase& other) {
    SetFromAnother(other);
    return *this;
}

void ProblemStateBase::SetFromAnother(const ProblemStateBase &other) {
    Lock lock(mutex_);
    valid_ = other.valid_;
    bool add = this > &other;
    u_longlong_t offset = add ? ((u_longlong_t)this - (u_longlong_t)&other) : ((u_longlong_t)&other - (u_longlong_t)this);
    u_longlong_t base_addr = (u_longlong_t)this;
    u_longlong_t limit = base_addr + sizeof(ProblemStateBase);

    CopyPointer(head_, other.head_, offset, add, base_addr, limit);
    CopyPointer(tail_, other.tail_, offset, add, base_addr, limit);
    for (uint_t i = 0; i < N_GRID; ++i) {
        CopyPointer(ele_list_[i].next_, other.ele_list_[i].next_, offset, add, base_addr, limit);
        CopyPointer(ele_list_[i].prev_, other.ele_list_[i].prev_, offset, add, base_addr, limit); 
        CopyPointer(ele_list_[i].state_, other.ele_list_[i].state_, offset, add, base_addr, limit);
        
        ele_arr_[i].SetFromAnother(other.ele_arr_[i], offset, add, base_addr, limit);
    }
}


void ProblemStateBase::SubscribePeers(uint_t y_idx, uint_t x_idx) {
    Lock lock(mutex_);
    ElementState *cur = &ele_arr_[IDX2OFFSET(y_idx, x_idx)];
    uint_t blk_y_start = y_idx / SUB_SIZE * SUB_SIZE;
    uint_t blk_x_start = x_idx / SUB_SIZE * SUB_SIZE;
    
    for (uint_t i = 0; i < SIZE; ++i) {
        if (i != x_idx) {
            cur->Subscribe(&ele_arr_[IDX2OFFSET(y_idx, i)]);
        }
    }

    for (uint_t i = 0; i < SIZE; ++i) {
        if (i != y_idx) {
            cur->Subscribe(&ele_arr_[IDX2OFFSET(i, x_idx)]);
        }
    }

    for (uint_t i = blk_y_start; i < blk_y_start + SUB_SIZE; ++i) {
        for (uint_t j = blk_x_start; j < blk_x_start + SUB_SIZE; ++j) {
            if (i != y_idx && j != x_idx) {
                cur->Subscribe(&ele_arr_[IDX2OFFSET(i, j)]);
            }
        }
    }
}

bool ProblemStateBase::Set(uint_t y_idx, uint_t x_idx, Element val) {
    Lock lock(mutex_);
    ElementState *node = &ele_arr_[IDX2OFFSET(y_idx, x_idx)];
    SUDOKU_ASSERT(node->val_ == UNFILLED);

    if (node->constraints_[val]) {
        return false;
    }

    node->val_ = val;
    // node->NotifyTaken(val);
    node->UnSubcribeAllForCur();
    RemoveFromList(head_, tail_, &ele_list_[IDX2OFFSET(y_idx, x_idx)]);
    bool ret = node->NotifySubscriberConstraints();
    if (!ret)
        valid_ = false;
    return ret;
}


// only used when you know it's correct for sure
void ProblemStateBase::SetAnswer(uint_t y_idx, uint_t x_idx, Element val) {
    Lock lock(mutex_);
    ElementState *node = &ele_arr_[IDX2OFFSET(y_idx, x_idx)];
    node->val_ = val;
}

uint_t ProblemStateBase::GetIdxWithMinPossibility(uint_t &y_idx, uint_t &x_idx) {
    Lock lock(mutex_);
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

bool ProblemStateBase::GetIdxFixedByPeers(uint_t &y_idx, uint_t &x_idx, Element &val) {
    Lock lock(mutex_);
    ElementListNode *cur = head_;
    while (cur != nullptr) {
        if (cur->state_->val_fix_ != UNFILLED) {
            y_idx = cur->state_->y_idx_;
            x_idx = cur->state_->x_idx_;
            val = cur->state_->val_fix_;
            return true;
        }
        cur = cur->next_;
    }
    return false;
}

uint_t ProblemStateBase::GetConstraints(uint_t y_idx, uint_t x_idx, bool *ret) {
    Lock lock(mutex_);
    const ElementState *node = &ele_arr_[IDX2OFFSET(y_idx, x_idx)];
    memcpy(ret, node->constraints_, sizeof(bool) * N_NUM);
    return node->n_possibilities_;
}

bool ProblemStateBase::SetConstraint(uint_t y_idx, uint_t x_idx, Element val) {
    Lock lock(mutex_);
    uint_t idx = IDX2OFFSET(y_idx, x_idx);
    bool ret = ele_arr_[idx].UpdateConstraints(val);
    if (!ret)
        valid_ = false;
    return ret;
}


}