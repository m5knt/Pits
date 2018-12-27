#include "../src/Encoding.hpp"
#include <uchar.h>
#include <cassert>
#include <string_view>
#include <iostream>

using namespace std::literals;

int main() {

    // ビルド環境で変わるので識別出来たかだけ確認
    static_assert(Pits::GetKanjiEncodingType("漢字") != Pits::EncodingType::OTHER);

    // ビルド環境で変わるので識別出来たかだけ確認
    static_assert(Pits::GetKanjiEncodingType(L"漢字") != Pits::EncodingType::OTHER);

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

    static_assert(Pits::Unicode::IsSurrogate(u"𐐷"[0]));
    static_assert(Pits::Unicode::IsSurrogate(u"𐐷"[1]));
    static_assert(Pits::Unicode::IsHighSurrogate(u"𐐷"[0]));
    static_assert(Pits::Unicode::IsLowSurrogate(u"𐐷"[1]));

    // C++20 なら static_assert 可能
    assert((Pits::EncodingUTF32ToUTF8Unsafe(U'𐐷') == std::array<char8_t, 4>{u8"𐐷"[0], u8"𐐷"[1], u8"𐐷"[2], u8"𐐷"[3]}));
    assert((Pits::EncodingUTF32ToUTF8Unsafe(U'漢') == std::array<char8_t, 4>{u8"漢"[0], u8"漢"[1], u8"漢"[2], u8"漢"[3]}));
    assert((Pits::EncodingUTF32ToUTF16Unsafe(U'𐐷') == std::array<char16_t, 2>{u"𐐷"[0], u"𐐷"[1]}));
    assert((Pits::EncodingUTF32ToUTF16Unsafe(U'漢') == std::array<char16_t, 2>{u"漢"[0], u"漢"[1]}));

    {
        constexpr auto a0 = Pits::EncodingUTF8ToUTF32(u8"𐐷漢字");
        static_assert(std::get<0>(a0) == U'𐐷');
        constexpr auto a1 = Pits::EncodingUTF8ToUTF32(std::get<1>(a0));
        static_assert(std::get<0>(a1) == U'漢');
        constexpr auto a2 = Pits::EncodingUTF8ToUTF32(std::get<1>(a1));
        static_assert(std::get<0>(a2) == U'字');
        static_assert(std::get<0>(Pits::EncodingUTF8ToUTF32("\xff\0")) == Pits::EncodingError);
    }
    {
        constexpr auto str = u"𐐷漢字";
        constexpr auto a0 = Pits::EncodingUTF16ToUTF32(u"𐐷漢字");
        static_assert(std::get<0>(a0) == U'𐐷');
        constexpr auto a1 = Pits::EncodingUTF16ToUTF32(std::get<1>(a0));
        static_assert(std::get<0>(a1) == U'漢');
        constexpr auto a2 = Pits::EncodingUTF16ToUTF32(std::get<1>(a1));
        static_assert(std::get<0>(a2) == U'字');
        static_assert(std::get<0>(Pits::EncodingUTF16ToUTF32(u"\xd800\0")) == Pits::EncodingError);
    }

    {
        auto sv = u8"𐐷漢字"sv;
        std::u32string out;
        out.reserve(sv.length());
        Pits::EncodingUTF8ToUTF32(sv.begin(), sv.end(), std::inserter(out, out.end()));
        assert(out == U"𐐷漢字");
    }
    {
        auto sv = u8"𐐷漢字"sv;
        std::u16string out;
        out.reserve(sv.length());
        Pits::EncodingUTF8ToUTF16(sv.begin(), sv.end(), std::inserter(out, out.end()));
        assert(out == u"𐐷漢字");
    }
    {
        auto sv = U"𐐷漢字"sv;
        std::u8string out;
        out.reserve(sv.length());
        Pits::EncodingUTF32ToUTF8(sv.begin(), sv.end(), std::inserter(out, out.end()));
        assert(out == u8"𐐷漢字");
    }
    {
        auto sv = u"𐐷漢字"sv;
        std::u8string out;
        out.reserve(sv.length());
        Pits::EncodingUTF16ToUTF8(sv.begin(), sv.end(), std::inserter(out, out.end()));
        assert(out == u8"𐐷漢字");
    }

#endif

    return EXIT_SUCCESS;
}
