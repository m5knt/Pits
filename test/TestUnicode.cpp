#include "Pits/Encoding.hpp"
#include "Pits/Timer.hpp"

#include <cassert>
#include <string_view>
#include <iostream>
#include <codecvt>

using namespace std::literals;

template <class UTF8Iterator,
    class Require = typename std::iterator_traits<UTF8Iterator>::value_type
>
constexpr auto EncodingUTF32ToUTF8Unsafe2(char32_t from, UTF8Iterator out) -> UTF8Iterator
{
    auto c = from;

    if (c <= 0x7f) {
        *out++ = char8_t(c);
    }
    else if (c <= 0x7ff) {
        *out++ = char8_t(((c >> 06) & 0b0'0011'1111) | 0b0'1100'0000);
        *out++ = char8_t(((c >> 00) & 0b0'0011'1111) | 0b0'1000'0000);
    }
    else if (c <= 0xffff) {
        *out++ = char8_t(((c >> 12) & 0b0'0001'1111) | 0b0'1110'0000);
        *out++ = char8_t(((c >> 06) & 0b0'0011'1111) | 0b0'1000'0000);
        *out++ = char8_t(((c >> 00) & 0b0'0011'1111) | 0b0'1000'0000);
    }
    else {
        *out++ = char8_t(((c >> 18) & 0b0'0000'1111) | 0b0'1111'0000);
        *out++ = char8_t(((c >> 12) & 0b0'0011'1111) | 0b0'1000'0000);
        *out++ = char8_t(((c >> 06) & 0b0'0011'1111) | 0b0'1000'0000);
        *out++ = char8_t(((c >> 00) & 0b0'0011'1111) | 0b0'1000'0000);
    }
    return out;
}

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
    {
        auto from = U"𐐷漢字";
        char8_t to[13] = {};
        auto f = from;
        auto t = to;
        std::tie(f, t) = Pits::Unicode::ConvertUTF32ToUTF8(f, t);
        std::tie(f, t) = Pits::Unicode::ConvertUTF32ToUTF8(f, t);
        std::tie(f, t) = Pits::Unicode::ConvertUTF32ToUTF8(f, t);
        assert(to == u8"𐐷漢字"s);
    }
    {
        auto from = U"𐐷漢字";
        char16_t to[13] = {};
        auto f = from;
        auto t = to;
        std::tie(f, t) = Pits::Unicode::ConvertUTF32ToUTF16(f, t);
        std::tie(f, t) = Pits::Unicode::ConvertUTF32ToUTF16(f, t);
        std::tie(f, t) = Pits::Unicode::ConvertUTF32ToUTF16(f, t);
        assert(to == u"𐐷漢字"s);
    }
    {
        auto from = u8"𐐷漢字";
        char32_t to[13] = {};
        auto f = from;
        auto t = to;
        std::tie(f, t) = Pits::Unicode::ConvertUTF8ToUTF32(f, t);
        std::tie(f, t) = Pits::Unicode::ConvertUTF8ToUTF32(f, t);
        std::tie(f, t) = Pits::Unicode::ConvertUTF8ToUTF32(f, t);
        assert(to == U"𐐷漢字"s);
    }
    {
        auto from = u"𐐷漢字";
        char32_t to[13] = {};
        auto f = from;
        auto t = to;
        std::tie(f, t) = Pits::Unicode::ConvertUTF16ToUTF32(f, t);
        std::tie(f, t) = Pits::Unicode::ConvertUTF16ToUTF32(f, t);
        std::tie(f, t) = Pits::Unicode::ConvertUTF16ToUTF32(f, t);
        assert(to == U"𐐷漢字"s);
    }

#endif

    return 0;
}
