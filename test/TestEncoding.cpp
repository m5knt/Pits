#include "../src/Encoding.hpp"
#include <cassert>

int main() {

    assert(Pits::GetEncodingType("漢") != Pits::EncodingType::OTHER);
    assert(Pits::GetEncodingType(L"漢") != Pits::EncodingType::OTHER);
	assert(Pits::GetEncodingType(u8"漢") == Pits::EncodingType::UTF8);
	assert(Pits::IsSurrogate(L'𐐷'));

    return EXIT_SUCCESS;
}
