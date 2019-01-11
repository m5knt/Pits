#include "Pits/Encoding.hpp"
#include "Pits/Timer.hpp"

#include <cassert>
#include <string_view>
#include <iostream>
#include <codecvt>

using namespace std::literals;

template <class Job>
void Bench(Job job) {
    Pits::Timer begin;
    job();
    std::cout << begin.GetElapsed() <<std::endl;
}

int main() {

#if defined(__STDC_UTF_16__) && defined(__STDC_UTF_32__)

    static_assert(Pits::Unicode::IsSurrogate(u"𐐷"[0]));
    static_assert(Pits::Unicode::IsSurrogate(u"𐐷"[1]));
    static_assert(Pits::Unicode::IsHighSurrogate(u"𐐷"[0]));
    static_assert(Pits::Unicode::IsLowSurrogate(u"𐐷"[1]));
    static_assert(Pits::Unicode::IsNotCharacter(U'\U0000fffe'));
    static_assert(Pits::Unicode::IsNotCharacter(U'\U0000ffff'));
    static_assert(Pits::Unicode::IsNotCharacter(U'\U0001ffff'));
    static_assert(Pits::Unicode::IsUnsafeCharacter(U'\x0000d800'));
    static_assert(Pits::Unicode::IsUnsafeCharacter(U'\x0000ffff'));
    static_assert(Pits::Unicode::IsUnsafeCharacter(U'\x00110000'));

    {
        auto from = U"𐐷漢字😀";
        char8_t to[100] = {};
        auto f = from;
        auto t = to;
        std::tie(f, t) = Pits::Unicode::ConvertUTF32ToUTF8(f, t);
        std::tie(f, t) = Pits::Unicode::ConvertUTF32ToUTF8(f, t);
        std::tie(f, t) = Pits::Unicode::ConvertUTF32ToUTF8(f, t);
        std::tie(f, t) = Pits::Unicode::ConvertUTF32ToUTF8(f, t);
        assert(to == u8"𐐷漢字😀"sv);
    }
    {
        auto from = U"𐐷漢字😀";
        char16_t to[100] = {};
        auto f = from;
        auto t = to;
        std::tie(f, t) = Pits::Unicode::ConvertUTF32ToUTF16(f, t);
        std::tie(f, t) = Pits::Unicode::ConvertUTF32ToUTF16(f, t);
        std::tie(f, t) = Pits::Unicode::ConvertUTF32ToUTF16(f, t);
        std::tie(f, t) = Pits::Unicode::ConvertUTF32ToUTF16(f, t);
        assert(to == u"𐐷漢字😀"sv);
    }
    {
        auto from = u8"𐐷漢字😀";
        char32_t to[100] = {};
        auto f = from;
        auto t = to;
        std::tie(f, t) = Pits::Unicode::ConvertUTF8ToUTF32(f, t);
        std::tie(f, t) = Pits::Unicode::ConvertUTF8ToUTF32(f, t);
        std::tie(f, t) = Pits::Unicode::ConvertUTF8ToUTF32(f, t);
        std::tie(f, t) = Pits::Unicode::ConvertUTF8ToUTF32(f, t);
        assert(to == U"𐐷漢字😀"sv);
    }
    {
        auto from = u"𐐷漢字😀";
        char32_t to[13] = {};
        auto f = from;
        auto t = to;
        std::tie(f, t) = Pits::Unicode::ConvertUTF16ToUTF32(f, t);
        std::tie(f, t) = Pits::Unicode::ConvertUTF16ToUTF32(f, t);
        std::tie(f, t) = Pits::Unicode::ConvertUTF16ToUTF32(f, t);
        std::tie(f, t) = Pits::Unicode::ConvertUTF16ToUTF32(f, t);
        assert(to == U"𐐷漢字😀"sv);
    }
    {
        auto from = u8"𐐷漢字😀";
        char16_t to[100] = {};
        auto f = from;
        auto t = to;
        std::tie(f, t) = Pits::Unicode::ConvertUTF8ToUTF16(f, t);
        std::tie(f, t) = Pits::Unicode::ConvertUTF8ToUTF16(f, t);
        std::tie(f, t) = Pits::Unicode::ConvertUTF8ToUTF16(f, t);
        std::tie(f, t) = Pits::Unicode::ConvertUTF8ToUTF16(f, t);
        assert(to == u"𐐷漢字😀"sv);
    }
    {
        auto from = u"𐐷漢字😀";
        char8_t to[100] = {};
        auto f = from;
        auto t = to;
        std::tie(f, t) = Pits::Unicode::ConvertUTF16ToUTF8(f, t);
        std::tie(f, t) = Pits::Unicode::ConvertUTF16ToUTF8(f, t);
        std::tie(f, t) = Pits::Unicode::ConvertUTF16ToUTF8(f, t);
        std::tie(f, t) = Pits::Unicode::ConvertUTF16ToUTF8(f, t);
        assert(to == u8"𐐷漢字😀"sv);
    }

#endif

    return 0;
}
