#ifndef SUDOKU_UTIL_PROBLEM_INFO_H_
#define SUDOKU_UTIL_PROBLEM_INFO_H_


#include "util/global.h"


namespace sudoku {

// only for chaning listnode that is already allocated

template <class T>
struct ListNode {
    ListNode() : state_(nullptr), next_(nullptr), prev_(nullptr) {};
    T *state_;
    ListNode *next_;
    ListNode *prev_;

};

template <class T>
void InsertIntoList(ListNode<T> *&head, ListNode<T> *&tail, ListNode<T> *insert) {
    SUDOKU_ASSERT(insert);
    if (head == nullptr) {
        head = insert;
        tail = insert;
        return;
    }

    SUDOKU_ASSERT(tail);
    tail->next_ = insert;
    insert->prev_ = tail;
    tail = tail->next_;
}


template <class T>
static void RemoveFromList(ListNode<T> *&head, ListNode<T> *&tail, ListNode<T> *remove) {
    SUDOKU_ASSERT(remove);
    SUDOKU_ASSERT(remove->state_ != nullptr);
    ListNode<T> *prev = remove->prev_;
    ListNode<T> *next = remove->next_;
    remove->next_ = nullptr;
    remove->state_ = nullptr;
    remove->prev_ = nullptr;

    if (prev != nullptr && next != nullptr) {
        prev->next_ = next;
        next->prev_ = prev;
        return;
    }

    if (prev == nullptr && next == nullptr) {
        SUDOKU_ASSERT(remove == head);
        SUDOKU_ASSERT(remove == tail);
        head = nullptr;
        tail = nullptr;
        return;
    }

    if (prev == nullptr) {
        SUDOKU_ASSERT(remove == head);
        head = next;
        head->prev_ = nullptr;
        return;
    }

    if (next == nullptr) {
        SUDOKU_ASSERT(remove == tail);
        tail = prev;
        tail->next_ = nullptr;
        return;
    }

}


}

#endif