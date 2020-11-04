#ifndef STRING_UTILS_H_
#define STRING_UTILS_H_

#include "util/global.h"
#include <sstream>

namespace sudoku {
void trim(std::string &str);

std::vector<std::string> split(const std::string &s, const std::string &delim);

std::vector<std::string> split_by_white(const std::string &s);

std::string join(const std::vector<std::string> str_vec, const std::string &delim);

std::string& replace_str(std::string& str, const std::string& to_replaced, const std::string& newchars);
}

#endif
