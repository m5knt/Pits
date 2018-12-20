#include "../src/Encoding.hpp"
#include <cuchar>
#include <cassert>
#include <string_view>
#include <iostream>

using namespace std::literals::string_view_literals;

int main() {

    // ビルド環境で変わるので識別出来たかだけ確認
    assert(Pits::GetEncodingType("漢字") != Pits::EncodingType::OTHER);

    // ビルド環境で変わるので識別出来たかだけ確認
    assert(Pits::GetEncodingType(L"漢字") != Pits::EncodingType::OTHER);

#if defined(__STDC_UTF_16__) && defined(__STDC_UTF_32__)

    std::cout << "defined __STDC_UTF_16__ __STDC_UTF_32__" << std::endl;

    assert(Pits::GetEncodingType(u8"漢字") == Pits::EncodingType::UTF8);
    assert(Pits::GetEncodingType(u8"\u6f22\u5b57") == Pits::EncodingType::UTF8);
    assert(Pits::GetEncodingType(u8"\U00006f22\U00005b57") == Pits::EncodingType::UTF8);

    assert(Pits::GetEncodingType(u"漢字") == Pits::EncodingType::UTF16);
    assert(Pits::GetEncodingType(u"\u6f22\u5b57") == Pits::EncodingType::UTF16);
    assert(Pits::GetEncodingType(u"\U00006f22\U00005b57") == Pits::EncodingType::UTF16);

    assert(Pits::GetEncodingType(U"漢字") == Pits::EncodingType::UTF32);
    assert(Pits::GetEncodingType(U"\u6f22\u5b57") == Pits::EncodingType::UTF32);
    assert(Pits::GetEncodingType(U"\U00006f22\U00005b57") == Pits::EncodingType::UTF32);

    assert(Pits::IsSurrogate(u"𐐷"[0]));
    assert(Pits::IsSurrogate(u"𐐷"[1]));
    assert(Pits::IsHighSurrogate(u"𐐷"[0]));
    assert(Pits::IsLowSurrogate(u"𐐷"[1]));

    {   
        auto sv = u8"𐐷漢字"sv;
        auto it = sv.begin();
        assert(Pits::UTF8ToUTF32(it, sv.end()) == U'𐐷');
        assert(Pits::UTF8ToUTF32(it, sv.end()) == U'漢');
        assert(Pits::UTF8ToUTF32(it, sv.end()) == U'字');
    }
    {
        auto sv = u"𐐷漢字"sv;
        auto it = sv.begin();
        assert(Pits::UTF16ToUTF32(it, sv.end()) == U'𐐷');
        assert(Pits::UTF16ToUTF32(it, sv.end()) == U'漢');
        assert(Pits::UTF16ToUTF32(it, sv.end()) == U'字');
    }

    assert((Pits::UTF32ToUTF16(U'𐐷') == std::array<char16_t, 2>{u"𐐷"[0], u"𐐷"[1]}));
    assert((Pits::UTF32ToUTF16(U'漢') == std::array<char16_t, 2>{u'漢', u'\0'}));

    assert((Pits::UTF32ToUTF8(U'𐐷') == std::array<char, 4>{u8"𐐷"[0], u8"𐐷"[1], u8"𐐷"[2], u8"𐐷"[3]}));
    assert((Pits::UTF32ToUTF8(U'漢') == std::array<char, 4>{u8"漢"[0], u8"漢"[1], u8"漢"[2], u8"漢"[3]}));

#endif

    return EXIT_SUCCESS;
}
