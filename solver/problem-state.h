#ifndef __PROBLEM_INFO__
#define __PROBLEM_INFO__

#include <queue>
#include <stack>
#include "util/global.h"
#include "itf/solvable-itf.h"


#define TEN_ONES 0x3FF


namespace sudoku {


// class for the current problem solving state

// TODO: declared as base since there could potentially be multiple serializaton implementations
// possible serialization:
//     1. output the sudoku matrix (min communication overhead)
//     2. output all the information includding the subscriber list
//        we can serielize the list using the element indices

// use stack for alloc this data structure because performance is the biggest priority
// if used with new for heap memory allocation, consider using a memory pool

class ProblemStateBase {

    struct ElementState;

    struct ElementListNode {
        ElementListNode() : state_(nullptr), next_(nullptr), prev_(nullptr) {};
        ElementState* state_;
        ElementListNode* next_;
        ElementListNode* prev_;
    };

    struct ElementState {

        ElementState() :
                val_(UNFILLED), n_possibilities_(N_NUM - 1),
                head_(nullptr), tail_(nullptr) {
            std::fill(peer_possibilities_array_, peer_possibilities_array_ + N_NUM, N_PEERS);
        }

        void Subscribe(ElementState* state);

        void UnSubScribe(ElementState *state);
        
        // return 0 if not solvable same as SetElement and PropConstraints
        // once the value is set, tell its peers
        bool NotifySubscriberConstraints();
        bool UpdateConstraints(Element val);
        // once the possiblity of a val is ruled out, tell its peers 
        bool NotifySubscriberPossibilities(Element val);
        bool UpdatePossibilities(Element val);

        // we don't want to send subsciber_idx_ through the network
        void ReconstructSubscriberIdx();

        Element val_;
        size_t x_idx_;
        size_t y_idx_;
        size_t n_possibilities_; // num of possiblities for the current element
       
        // if peer_possibilities_array[i] == n, it means n peers haven't rule out
        // the possibility of containing i, used for pruning (rule 2)
        // index 0 is meaningless, it is just used to be consistent with
        // the numbers we can fill in the sudoku matrix
        size_t peer_possibilities_array_[N_NUM];
        
        //std::bitset<N_NUM> constraints_;
        // faster access than bitset
        bool constraints_[N_NUM]; 

        // use observer pattern to distribute update information
        // the list support O(1) time lookup, deletion, insertion
        ElementListNode *head_;
        ElementListNode *tail_;
        ElementListNode subscriber_list_[N_PEERS];
        
        // for finding the idx of a particular subsciber in subscriber_list_ in O(1) time
        // used as a unordered_map but probably faster
        // very expensive to send the array over the network cause it takes O(N^2) size
        // but construting it only takes O(N) (O(N^2) if taking into acount of initialization)
        size_t subscriber_idx_[N_GRID];
        
    private:
        inline size_t GetIdxInSubList(size_t y_idx, size_t x_idx);
    };


public:

    ProblemStateBase(Solvable *problem);
    ProblemStateBase(Solvable &other);
    ProblemStateBase& operator=(const ProblemStateBase& other);

    ~ProblemStateBase();

    bool Set(size_t y_idx, size_t x_idx, Element val);

    // helper for using copy constructor for assignment
    friend void swap(ProblemStateBase &first, ProblemStateBase &second);

private:
    void SubscribePeers(size_t y_idx, size_t x_idx);

    void RemoveNode(ElementState *node);

    ElementState ele_arr_[N_GRID];

};


}

#endif