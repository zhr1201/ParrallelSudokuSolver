#ifndef _GLOBAL_H_
#define _GLOBAL_H_


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

#ifdef _MSC_VER
#define posix_memalign(pmemptr, alignment, size) (((*(pmemptr)) = _aligned_malloc((size), (alignment))), *(pmemptr) ? 0 : -1)
#define posix_memfree(pmemptr) (_aligned_free(pmemptr))
#else
#define posix_memfree(pmemptr) (free(pmemptr))
#endif

#ifdef WIN32
#include <crtdbg.h>
#define W_ASSERT				_ASSERT
#else
#define W_ASSERT				assert
#endif


// Gloabl macros
#define UNFILLED 0


// Global alias
using Element = uint8_t;
using SudokuMat = std::vector<std::vector<Element>>;


#endif // _GLOBAL_H_