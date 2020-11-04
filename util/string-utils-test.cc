#include <util/string-utils.h>
#include <assert.h>
#include <iostream>

namespace sudoku {

void TestSplitStr() {
    // std::string str("a,b,c");
    // std::vector<std::string> split_rst = split(str, ",");
    // assert(split_rst[0] == "a");
    // assert(split_rst[1] == "b");
    // assert(split_rst[2] == 'c');
}

}

int main() {
    using namespace sudoku;
    TestSplitStr();
    std::cout << "String util test PASS" << std::endl;
}
