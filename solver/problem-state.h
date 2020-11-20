<<<<<<< HEAD
=======
// sovler/problem-state.h (author: Haoran Zhou)

// Class for maintining the current problem solving state and provide prunning info
// the logic of prunning and searching is not implemented outside this class
// this could lead to a slightly worse performance but better software architecture

// !!!IMPORTANT!!!: use stack for alloc the memory because performance is the top priority
// Heap allocation is extreamly slow. Don't use STL containers that could potentional use the new operator or malloc
// If dynamic heap memory allocation is neccesary, consider using a global memory pool

// TODO: declared as base since there could potentially be multiple serializaton implementations
// possible serialization:
//     1. output the sudoku matrix (min communication overhead)
//     2. output all the information includding the subscriber list
//        we can serielize the list using the element index


>>>>>>> d1fb5f5db3ce0dba3bac9cc4e2d8e0080366a2ed
#ifndef __PROBLEM_INFO__
#define __PROBLEM_INFO__

#include <queue>
#include <stack>
<<<<<<< HEAD
=======

>>>>>>> d1fb5f5db3ce0dba3bac9cc4e2d8e0080366a2ed
#include "util/global.h"
#include "itf/solvable-itf.h"


<<<<<<< HEAD
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

=======
namespace sudoku {


>>>>>>> d1fb5f5db3ce0dba3bac9cc4e2d8e0080366a2ed
class ProblemStateBase {

    struct ElementState;

    struct ElementListNode {
        ElementListNode() : state_(nullptr), next_(nullptr), prev_(nullptr) {};
        ElementState* state_;
        ElementListNode* next_;
        ElementListNode* prev_;
    };

<<<<<<< HEAD
    struct ElementState {

        ElementState() :
                val_(UNFILLED), n_possibilities_(N_NUM - 1),
                head_(nullptr), tail_(nullptr) {
=======
    // private list helper
    static void InsertIntoList(ElementListNode *&head, ElementListNode *&tail, ElementListNode *insert);

    static void RemoveFromList(ElementListNode *&head, ElementListNode *&tail, ElementListNode *remove);
 
    struct ElementState {

        ElementState() :
                val_(UNFILLED), n_possibilities_(N_NUM - 1), val_fix_(UNFILLED),
                constraints_(), head_(nullptr), tail_(nullptr), subscriber_list_(), subscriber_idx_() {
>>>>>>> d1fb5f5db3ce0dba3bac9cc4e2d8e0080366a2ed
            std::fill(peer_possibilities_array_, peer_possibilities_array_ + N_NUM, N_PEERS);
        }

        void Subscribe(ElementState* state);

<<<<<<< HEAD
        void UnSubScribe(ElementState *state);
=======
        void UnSubscribe(ElementState *state);
        // subsubscribe the current node from all peers
        void UnSubcribeAllForCur();
>>>>>>> d1fb5f5db3ce0dba3bac9cc4e2d8e0080366a2ed
        
        // return 0 if not solvable same as SetElement and PropConstraints
        // once the value is set, tell its peers
        bool NotifySubscriberConstraints();
        bool UpdateConstraints(Element val);
        // once the possiblity of a val is ruled out, tell its peers 
        bool NotifySubscriberPossibilities(Element val);
        bool UpdatePossibilities(Element val);

        // we don't want to send subsciber_idx_ through the network
<<<<<<< HEAD
        void ReconstructSubscriberIdx();
=======
        void ConstructSubscriberIdx();
>>>>>>> d1fb5f5db3ce0dba3bac9cc4e2d8e0080366a2ed

        Element val_;
        size_t x_idx_;
        size_t y_idx_;
        size_t n_possibilities_; // num of possiblities for the current element
       
        // if peer_possibilities_array[i] == n, it means n peers haven't rule out
        // the possibility of containing i, used for pruning (rule 2)
        // index 0 is meaningless, it is just used to be consistent with
        // the numbers we can fill in the sudoku matrix
        size_t peer_possibilities_array_[N_NUM];
<<<<<<< HEAD
=======
        // the value is set to x if peer_possibilities_array_[x] == 0
        // but the logic of setting val_ is left for the solver class 
        Element val_fix_;
>>>>>>> d1fb5f5db3ce0dba3bac9cc4e2d8e0080366a2ed
        
        //std::bitset<N_NUM> constraints_;
        // faster access than bitset
        bool constraints_[N_NUM]; 

<<<<<<< HEAD
        // use observer pattern to distribute update information
        // the list support O(1) time lookup, deletion, insertion
=======
        // use observer design pattern to broadcast update information
        // the list support O(1) time lookup, deletion, insertion
        // similar to hashmap but with O(1) worst case lookup and a fixed memory usage, easier to serialize
>>>>>>> d1fb5f5db3ce0dba3bac9cc4e2d8e0080366a2ed
        ElementListNode *head_;
        ElementListNode *tail_;
        ElementListNode subscriber_list_[N_PEERS];
        
        // for finding the idx of a particular subsciber in subscriber_list_ in O(1) time
        // used as a unordered_map but probably faster
<<<<<<< HEAD
        // very expensive to send the array over the network cause it takes O(N^2) size
=======
        // very expensive to send the array over the network cause it takes O(N^2) space
>>>>>>> d1fb5f5db3ce0dba3bac9cc4e2d8e0080366a2ed
        // but construting it only takes O(N) (O(N^2) if taking into acount of initialization)
        size_t subscriber_idx_[N_GRID];
        
    private:
        inline size_t GetIdxInSubList(size_t y_idx, size_t x_idx);
    };


public:

    ProblemStateBase(Solvable *problem);
<<<<<<< HEAD
    ProblemStateBase(Solvable &other);
    ProblemStateBase& operator=(const ProblemStateBase& other);

    ~ProblemStateBase();

    bool Set(size_t y_idx, size_t x_idx, Element val);

    // helper for using copy constructor for assignment
    friend void swap(ProblemStateBase &first, ProblemStateBase &second);
=======
    // deep copy constructor
    ProblemStateBase(ProblemStateBase &other);

    // copy-and-swap
    ProblemStateBase& operator=(const ProblemStateBase& other);

    // helper for copy-and-swap idom
    friend void swap(ProblemStateBase &first, ProblemStateBase &second) {
        // enable ADL
        using std::swap;
        swap(first.ele_arr_, second.ele_arr_);
        swap(first.valid_, second.valid_);
        swap(first.ele_list_, second.ele_list_);
        swap(first.head_, second.head_);
        swap(first.tail_, second.tail_);
    };

    bool CheckValid() { return valid_; };

    // worst case O(N ^ 2) to prop constraints but get smaller near the end
    bool Set(size_t y_idx, size_t x_idx, Element val);

    // prune criteria 1
    // returns num possibility and indices
    // worst case O(N ^ 2), priority queue can be used but will cost O (N log N) for each Set operation and N is only 9
    size_t GetIdxWithMinPossibility(size_t &y_idx, size_t &x_idx) const; 

    // prune criteria 2
    // all other peers force the current element to take a certain value
    bool GetIdxFixedByPeers(size_t &y_idx, size_t &x_idx, Element &val) const;
>>>>>>> d1fb5f5db3ce0dba3bac9cc4e2d8e0080366a2ed

private:
    void SubscribePeers(size_t y_idx, size_t x_idx);

<<<<<<< HEAD
    void RemoveNode(ElementState *node);

    ElementState ele_arr_[N_GRID];

=======
    ElementState ele_arr_[N_GRID];    
    bool valid_;

    // list for unset elements
    ElementListNode ele_list_[N_GRID];
    ElementListNode *head_;
    ElementListNode *tail_;
>>>>>>> d1fb5f5db3ce0dba3bac9cc4e2d8e0080366a2ed
};


}

#endif