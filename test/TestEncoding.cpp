#include "../src/Encoding.hpp"
#include <uchar.h>
#include <cassert>
#include <string_view>
#include <iostream>

using namespace std::literals::string_view_literals;

int main() {

    // ビルド環境で変わるので識別出来たかだけ確認
    static_assert(Pits::GetKanjiEncodingType("漢字") != Pits::EncodingType::OTHER);

    // ビルド環境で変わるので識別出来たかだけ確認
    static_assert(Pits::GetKanjiEncodingType(L"漢字") != Pits::EncodingType::OTHER);

// char16_t, char32_t が UTF ではない環境がいかほどあるのか謎だけどとりあえず
#if defined(__STDC_UTF_16__) && defined(__STDC_UTF_32__)

    std::cout << "defined __STDC_UTF_16__ __STDC_UTF_32__" << std::endl;

    static_assert(Pits::GetKanjiEncodingType(u8"漢字") == Pits::EncodingType::UTF8);
    static_assert(Pits::GetKanjiEncodingType(u8"\u6f22\u5b57") == Pits::EncodingType::UTF8);
    static_assert(Pits::GetKanjiEncodingType(u8"\U00006f22\U00005b57") == Pits::EncodingType::UTF8);

    static_assert(Pits::GetKanjiEncodingType(u"漢字") == Pits::EncodingType::UTF16);
    static_assert(Pits::GetKanjiEncodingType(u"\u6f22\u5b57") == Pits::EncodingType::UTF16);
    static_assert(Pits::GetKanjiEncodingType(u"\U00006f22\U00005b57") == Pits::EncodingType::UTF16);

    static_assert(Pits::GetKanjiEncodingType(U"漢字") == Pits::EncodingType::UTF32);
    static_assert(Pits::GetKanjiEncodingType(U"\u6f22\u5b57") == Pits::EncodingType::UTF32);
    static_assert(Pits::GetKanjiEncodingType(U"\U00006f22\U00005b57") == Pits::EncodingType::UTF32);

    static_assert(Pits::IsSurrogate(u"𐐷"[0]));
    static_assert(Pits::IsSurrogate(u"𐐷"[1]));
    static_assert(Pits::IsHighSurrogate(u"𐐷"[0]));
    static_assert(Pits::IsLowSurrogate(u"𐐷"[1]));

    // C++17 には constexpr 比較が無いので
    assert((Pits::UTF32ToUTF16(U'𐐷') == std::array<char16_t, 2>{u"𐐷"[0], u"𐐷"[1]}));
    assert((Pits::UTF32ToUTF16(U'漢') == std::array<char16_t, 2>{u"漢"[0], u"漢"[1]}));
    assert((Pits::UTF32ToUTF8(U'𐐷') == std::array<char, 4>{u8"𐐷"[0], u8"𐐷"[1], u8"𐐷"[2], u8"𐐷"[3]}));
    assert((Pits::UTF32ToUTF8(U'漢') == std::array<char, 4>{u8"漢"[0], u8"漢"[1], u8"漢"[2], u8"漢"[3]}));
    {
        constexpr auto a0 = Pits::UTF8ToUTF32(u8"𐐷漢字");
        static_assert(a0.first == U'𐐷');
        constexpr auto a1 = Pits::UTF8ToUTF32(a0.second);
        static_assert(a1.first == U'漢');
        constexpr auto a2 = Pits::UTF8ToUTF32(a1.second);
        static_assert(a2.first == U'字');
    }
    {
        constexpr auto a0 = Pits::UTF16ToUTF32(u"𐐷漢字");
        static_assert(a0.first == U'𐐷');
        constexpr auto a1 = Pits::UTF16ToUTF32(a0.second);
        static_assert(a1.first == U'漢');
        constexpr auto a2 = Pits::UTF16ToUTF32(a1.second);
        static_assert(a2.first == U'字');
    }

#endif

    return EXIT_SUCCESS;
}
