#include "Pits/StringStream.hpp"
#include <sstream>
#include <iostream>
#include <cassert>

int main() {

    Pits::BasicStringStream<char> ss;
    ss << "ABC";   
    assert(ss.str() == ss.stringstream::str());
    char c;
    ss >> c;
    assert(ss.str() == ss.stringstream::str());
    return 0;
}
