#ifndef SUDOKU_UTIL_GLOBAL_H_
#define SUDOKU_UTIL_GLOBAL_H_


#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <vector>
#include <string>
#include <list>
#include <map>
#include <cassert>
#include <algorithm>
#include <numeric>
#include <cfloat>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <stdint.h>

#define DISALLOW_CLASS_COPY_AND_ASSIGN(type) type(const type &); type& operator=(const type &)

#ifndef NDEBUG
#define SUDOKU_ASSERT(cond) assert(cond)
#else
#define SUDOKU_ASSERT(cond) (void)0
#endif


#ifdef _MSC_VER
#define posix_memalign(pmemptr, alignment, size) (((*(pmemptr)) = _aligned_malloc((size), (alignment))), *(pmemptr) ? 0 : -1)
#define posix_memfree(pmemptr) (_aligned_free(pmemptr))
#else
#define posix_memfree(pmemptr) (free(pmemptr))
#endif

#ifndef u_longlong_t
typedef unsigned long long u_longlong_t;
#endif


// Gloabl macros
#define UNFILLED 0
#define SIZE 9
#define N_BLOCKS 9
#define N_BLOCKS_1D 3
#define N_GRID 81
#define SUB_SIZE 3
#define N_NUM 10
#define N_PEERS 20

// macro functions
#define IDX2OFFSET(Y, X)  (Y * SIZE + X)


// Global alias
using Element = size_t;
using SudokuMat = std::vector<std::vector<Element>>;


#endif // _GLOBAL_H_