#include "../src/Encoding.hpp"
#include <cassert>
#include <string_view>

using namespace std::literals::string_view_literals;

int main() {

    assert(Pits::GetEncodingType("漢") != Pits::EncodingType::OTHER);
    assert(Pits::GetEncodingType(L"漢") != Pits::EncodingType::OTHER);
	assert(Pits::GetEncodingType(u8"漢") == Pits::EncodingType::UTF8);
	assert(Pits::IsSurrogate(L"𐐷"[0]));

    {   
        auto sv = u8"漢"sv;
        assert(Pits::UTF8ToUTF32(sv.begin(), sv.end()) == U'漢');
    }
    {
        auto sv = u"𐐷"sv;
        assert(Pits::UTF16ToUTF32(sv.begin(), sv.end()) == U'𐐷');
    }
    return EXIT_SUCCESS;
}
