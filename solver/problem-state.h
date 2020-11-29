<<<<<<< HEAD
=======
// sovler/problem-state.h (author: Haoran Zhou)

// Class for maintining the current problem solving state and provide prunning info
// the logic of prunning and searching should be implemented outside this class
// this could lead to a slightly worse performance but better software architecture

// !!!IMPORTANT!!!: use static array for alloc the memory in the solving phase because performance is the top priority
// Heap allocation is extreamly slow. Don't use STL containers that could potentional use the new operator or malloc
// If dynamic heap memory allocation is neccesary, consider using a memory pool

// TODO: declared as base since there could potentially be multiple serializaton implementations
// possible serialization:
//     1. output the sudoku matrix (min communication overhead)
//     2. output all the information includding the subscriber list
//        we can serielize the list using the element index


<<<<<<< HEAD
>>>>>>> d1fb5f5db3ce0dba3bac9cc4e2d8e0080366a2ed
#ifndef __PROBLEM_INFO__
#define __PROBLEM_INFO__
=======
#ifndef SUDOKU_SOLVER_PROBLEM_INFO_H_
#define SUDOKU_SOLVER_PROBLEM_INFO_H_
>>>>>>> solver

#include <queue>
#include <stack>
<<<<<<< HEAD
=======

>>>>>>> d1fb5f5db3ce0dba3bac9cc4e2d8e0080366a2ed
#include "util/global.h"
#include "itf/solvable-itf.h"
#include "util/list-utils.h"
#include "util/mutex.h"


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

    // private helper function and data structures

<<<<<<< HEAD
<<<<<<< HEAD
    struct ElementState {

        ElementState() :
                val_(UNFILLED), n_possibilities_(N_NUM - 1),
                head_(nullptr), tail_(nullptr) {
=======
    // private list helper
    static void InsertIntoList(ElementListNode *&head, ElementListNode *&tail, ElementListNode *insert);
=======
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
>>>>>>> solver

        SUDOKU_ASSERT(base <= tmp && tmp < limit);
        dst = (T*)tmp;
    }
 
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
<<<<<<< HEAD
>>>>>>> d1fb5f5db3ce0dba3bac9cc4e2d8e0080366a2ed
=======
        bool NotifyTaken(Element val);
        void Clear();
        // set value from another instance, should only get called by the copy constructor of ProblemState
        void SetFromAnother(const ElementState &other, u_longlong_t offset, bool add, u_longlong_t base, u_longlong_t limit);
>>>>>>> solver

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
        DISALLOW_CLASS_COPY_AND_ASSIGN(ElementState);
    };


public:    
    // start of ProblemStateBase def 
    ProblemStateBase() {};

    ProblemStateBase(const Solvable *problem);

<<<<<<< HEAD
    ProblemStateBase(Solvable *problem);
<<<<<<< HEAD
    ProblemStateBase(Solvable &other);
    ProblemStateBase& operator=(const ProblemStateBase& other);

    ~ProblemStateBase();

    bool Set(size_t y_idx, size_t x_idx, Element val);

    // helper for using copy constructor for assignment
    friend void swap(ProblemStateBase &first, ProblemStateBase &second);
=======
=======
>>>>>>> solver
    // deep copy constructor
    ProblemStateBase(const ProblemStateBase &other);

    void ResetProblem(const Solvable *problem);

    // can't use copy-and-swap idom here cause the pointers variable is not allocated using new
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
    size_t GetIdxWithMinPossibility(size_t &y_idx, size_t &x_idx);

    // prune criteria 2
    // all other peers force the current element to take a certain value
<<<<<<< HEAD
    bool GetIdxFixedByPeers(size_t &y_idx, size_t &x_idx, Element &val) const;
>>>>>>> d1fb5f5db3ce0dba3bac9cc4e2d8e0080366a2ed
=======
    bool GetIdxFixedByPeers(size_t &y_idx, size_t &x_idx, Element &val);

    size_t GetConstraints(size_t y_idx, size_t x_idx, bool *ret);

    // void SanitiCheck() {
    //     ElementListNode *cur = head_;
    //     while (cur != tail_) {
    //         SUDOKU_ASSERT(cur->state_);
    //         cur = cur->next_;
    //     }

    //     for (size_t i = 0; i < N_GRID; ++i) {
    //         if (ele_list_[i].state_ == nullptr) {
    //             SUDOKU_ASSERT(ele_arr_[i].val_ != UNFILLED);
    //         } else {
    //             SUDOKU_ASSERT(ele_arr_[i].val_ == UNFILLED);
    //         }
    //     }
    // }

    // could be used by passing constraints between processes
    bool SetConstraint(size_t y_idx, size_t x_idx, Element val); 
>>>>>>> solver

private:
    void SubscribePeers(size_t y_idx, size_t x_idx);
    void SetFromAnother(const ProblemStateBase &other);
    void Clear();
    void SetProblem(const Solvable *problem);

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
<<<<<<< HEAD
>>>>>>> d1fb5f5db3ce0dba3bac9cc4e2d8e0080366a2ed
=======

    // Thread safe in case we need to combine MPI with pthread
    Mutex mutex_;
>>>>>>> solver
};


}

#endif