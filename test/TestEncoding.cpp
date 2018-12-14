#include "../src/Encoding.hpp"
#include <cassert>

int main() {

    assert(Pits::GetEncodingType("漢") != Pits::EncodingType::OTHER);
    assert(Pits::GetEncodingType(L"漢") != Pits::EncodingType::OTHER);

    return EXIT_SUCCESS;
}
