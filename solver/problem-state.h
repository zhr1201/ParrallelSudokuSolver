// sovler/problem-state.h (author: Haoran Zhou)

// Class for maintining the current problem solving state and provide prunning info
// the logic of prunning and searching should be implemented outside this class
// this could lead to a slightly worse performance but better software architecture

// !!!IMPORTANT!!!: use stack for alloc the memory because performance is the top priority
// Heap allocation is extreamly slow. Don't use STL containers that could potentional use the new operator or malloc
// If dynamic heap memory allocation is neccesary, consider using a global memory pool

// TODO: declared as base since there could potentially be multiple serializaton implementations
// possible serialization:
//     1. output the sudoku matrix (min communication overhead)
//     2. output all the information includding the subscriber list
//        we can serielize the list using the element index


#ifndef SUDOKU_SOLVER_PROBLEM_INFO_H_
#define SUDOKU_SOLVER_PROBLEM_INFO_H_

#include <queue>
#include <stack>

#include "util/global.h"
#include "itf/solvable-itf.h"
#include "util/list-utils.h"


namespace sudoku {


class ProblemStateBase {

    // private helper function and data structures

    struct ElementState;
    using ElementListNode = ListNode<ElementState>;

    // set dst with an offset + src, T could be ElementListNode or ElementState here
    template <typename T>
    static void CopyPointer(T *&dst, void *src, u_longlong_t offset, bool add, u_longlong_t base, u_longlong_t limit) {
        if (src == nullptr) {
            dst = nullptr;
            return;
        }
        u_longlong_t tmp;
        if (add) {
            tmp = (u_longlong_t)src + (u_longlong_t)offset;
        } else {
            tmp = (u_longlong_t)src - (u_longlong_t)offset;
        }

        SUDOKU_ASSERT(base <= tmp && tmp < limit);
        dst = (T*)tmp;
    }
 
    struct ElementState {

        ElementState() :
                val_(UNFILLED), n_possibilities_(N_NUM - 1), val_fix_(UNFILLED),
                constraints_(), head_(nullptr), tail_(nullptr), subscriber_list_(), subscriber_idx_() {
            std::fill(peer_possibilities_array_, peer_possibilities_array_ + N_NUM, N_PEERS);
        }

        void Subscribe(ElementState* state);

        void UnSubscribe(ElementState *state);
        // subsubscribe the current node from all peers
        void UnSubcribeAllForCur();
        
        // return 0 if not solvable same as SetElement and PropConstraints
        // once the value is set, tell its peers
        bool NotifySubscriberConstraints();
        bool UpdateConstraints(Element val);
        // once the possiblity of a val is ruled out, tell its peers 
        bool NotifySubscriberPossibilities(Element val);
        bool UpdatePossibilities(Element val);

        // we don't want to send subsciber_idx_ through the network
        void ConstructSubscriberIdx();

        // set value from another instance, should only get called by the copy constructor of ProblemState
        void SetFromAnother(const ElementState &other, u_longlong_t offset, bool add, u_longlong_t base, u_longlong_t limit);

        Element val_;
        size_t x_idx_;
        size_t y_idx_;
        size_t n_possibilities_; // num of possiblities for the current element
       
        // if peer_possibilities_array[i] == n, it means n peers haven't rule out
        // the possibility of containing i, used for pruning (rule 2)
        // index 0 is meaningless, it is just used to be consistent with
        // the numbers we can fill in the sudoku matrix
        size_t peer_possibilities_array_[N_NUM];
        // the value is set to x if peer_possibilities_array_[x] == 0
        // but the logic of setting val_ is left for the solver class 
        Element val_fix_;
        
        //std::bitset<N_NUM> constraints_;
        // faster access than bitset
        bool constraints_[N_NUM]; 

        // use observer design pattern to broadcast update information
        // the list support O(1) time lookup, deletion, insertion
        // similar to hashmap but with O(1) worst case lookup and a fixed memory usage, easier to serialize
        ElementListNode *head_;
        ElementListNode *tail_;
        ElementListNode subscriber_list_[N_PEERS];
        
        // for finding the idx of a particular subsciber in subscriber_list_ in O(1) time
        // used as a unordered_map but probably faster
        // very expensive to send the array over the network cause it takes O(N^2) space
        // but construting it only takes O(N) (O(N^2) if taking into acount of initialization)
        size_t subscriber_idx_[N_GRID];
        
    private:
        inline size_t GetIdxInSubList(size_t y_idx, size_t x_idx);
        DISALLOW_CLASS_COPY_AND_ASSIGN(ElementState);
    };


public:    
    // start of ProblemStateBase def 
    ProblemStateBase() {};

    ProblemStateBase(const Solvable *problem);

    // deep copy constructor
    ProblemStateBase(const ProblemStateBase &other);

    // can't use copy-and-swap idom here cause the pointers variable is not on heap
    // and is pointing to some address within the data structure
    ProblemStateBase& operator=(const ProblemStateBase& other);

    // TODO: refac to one state check call
    bool CheckValid() const { return valid_; };

    bool CheckSolved() const { return valid_ && (!head_); };

    // worst case O(N ^ 2) to prop constraints but get smaller near the end
    bool Set(size_t y_idx, size_t x_idx, Element val);

    Element Get(size_t y_idx, size_t x_idx) const { return ele_arr_[IDX2OFFSET(y_idx, x_idx)].val_; };

    // prune criteria 1
    // returns num possibility and indices
    // worst case O(N ^ 2), priority queue can be used but will cost O (N log N) for each Set operation and N is only 9
    size_t GetIdxWithMinPossibility(size_t &y_idx, size_t &x_idx) const;

    // prune criteria 2
    // all other peers force the current element to take a certain value
    bool GetIdxFixedByPeers(size_t &y_idx, size_t &x_idx, Element &val) const;

    size_t GetConstraints(size_t y_idx, size_t x_idx, bool *ret) const;

    void SanitiCheck() {
        ElementListNode *cur = head_;
        while (cur != tail_) {
            SUDOKU_ASSERT(cur->state_);
            cur = cur->next_;
        }

        for (size_t i = 0; i < N_GRID; ++i) {
            if (ele_list_[i].state_ == nullptr) {
                SUDOKU_ASSERT(ele_arr_[i].val_ != UNFILLED);
            } else {
                SUDOKU_ASSERT(ele_arr_[i].val_ == UNFILLED);
            }
        }
    }

private:
    void SubscribePeers(size_t y_idx, size_t x_idx);
    void SetFromAnother(const ProblemStateBase &other);

    ElementState ele_arr_[N_GRID];    
    bool valid_;

    // list for unset elements
    ElementListNode ele_list_[N_GRID];
    ElementListNode *head_;
    ElementListNode *tail_;
};


}

#endif