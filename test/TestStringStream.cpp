#include "../src/StringStream.hpp"
#include <sstream>
#include <iostream>
#include <cassert>

int main() {

    std::stringstream ss;
    ss << "ABC";   
    assert(Pits::ToStringView(ss) == ss.str());
    char c;
    ss >> c;
    assert(Pits::ToStringView(ss) == ss.str());
    assert(Pits::ToStringView((std::stringbuf*)0) == std::string_view());
    return 0;
}
