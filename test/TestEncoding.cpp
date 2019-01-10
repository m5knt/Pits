#include "Pits/Encoding.hpp"
#include "Pits/Timer.hpp"
#include <uchar.h>
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

    std::cout << "defined __STDC_UTF_16__ __STDC_UTF_32__" << std::endl;

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
        static_assert(std::get<0>(Pits::EncodingUTF8ToUTF32("\xff\0")) == Pits::EncodingErrorIllegalSequence);
    }
    {
        constexpr auto a0 = Pits::EncodingUTF16ToUTF32(u"𐐷漢字");
        static_assert(std::get<0>(a0) == U'𐐷');
        constexpr auto a1 = Pits::EncodingUTF16ToUTF32(std::get<1>(a0));
        static_assert(std::get<0>(a1) == U'漢');
        constexpr auto a2 = Pits::EncodingUTF16ToUTF32(std::get<1>(a1));
        static_assert(std::get<0>(a2) == U'字');
        static_assert(std::get<0>(Pits::EncodingUTF16ToUTF32(u"\xd800\0")) == Pits::EncodingErrorIllegalSequence);
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
    {
        auto sv = "\xC0\xAF\xE0\x80\xBF\xF0\x81\x82\x41"sv;
        // [2] c0 af        => 00 2F    => 002f => RC RC    非最短
        // [3] e0 80 bf     => 00 00 3F => 003f => RC RC RC 非最短
        // [4] f0 81 82 41x => 01 02 xx => 1080 => RC RC RC *最終+3にASCII不正シーケンス
        // [1] 41           => 41       => 41
        std::u16string out;
        Pits::EncodingUTF8ToUTF16(sv.begin(), sv.end(), std::inserter(out, out.end()));
        assert(out == u"\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\u0041");
        //assert(out == u"\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\u0041");
    }
    {
        auto sv = "\xED\xA0\x80\xED\xBF\xBF\xED\xAF\x41"sv;
        // [3] ed a0 80  => D 20 00 => D800 => RC RC RC サロゲート
        // [3] ed bf bf  => D 3F 3F => DFFF => RC RC RC サロゲート
        // [3] ed af 41x => D 2F xx => DBC0 => RC RC    *中途+2にASCII不正シーケンス
        // [1] 41        => 0041 => 41
        std::u16string out;
        Pits::EncodingUTF8ToUTF16(sv.begin(), sv.end(), std::inserter(out, out.end()));
        assert(out == u"\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\u0041");
        //assert(out == u"\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\u0041");
    }
    {
        auto sv = "\xF4\x91\x92\x93\xFF\x41\x80\xBF\x42"sv;
        // [4] f4 91 92 93 => 7 11 12 13 => 1D1493 => RC RC RC RC 大きい
        // [-] ff          =>                      => RC          開始ではない 
        // [1] 41                                  => 41
        // [-] 80                                  => RC          開始ではない                    
        // [-] bf                                  => RC          不正シーケンス
        // [1] 42                                  => 42
        std::u16string out;
        Pits::EncodingUTF8ToUTF16(sv.begin(), sv.end(), std::inserter(out, out.end()));
        assert(out == u"\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\u0041\uFFFD\uFFFD\u0042");
    }
    {
        auto sv = "\xE1\x80\xE2\xF0\x91\x92\xF1\xBF\x41"sv;
        // [3] e1 80 e2x    => 01 00 xx    =>  1000 => RC 中途+2に不正シーケンス
        // [3] e2 f0x       => 02 xx       =>  2000 => RC 中途+2に不正シーケンス
        // [4] f0 91 92 f1x => 00 11 12 xx => 11480 => RC 中途+3に不正シーケンス
        // [4] f1 bf 41x    => 01 3f xx xx => 7F000 => RC *中途+2にASCII不正シーケンス
        // [1] 41           => 41
        std::u16string out;
        Pits::EncodingUTF8ToUTF16(sv.begin(), sv.end(), std::inserter(out, out.end()));
        assert(out == u"\uFFFD\uFFFD\uFFFD\uFFFD\u0041");
    }
#endif

    // std::wifstream fin("text.txt");
    //     // reading from wifstream will use codecvt<wchar_t, char, mbstate_t>
    //     // this locale's codecvt converts UTF-8 to UCS4 (on systems such as Linux)
    //     fin.imbue(std::locale("en_US.UTF-8"));
    //     std::cout << "The UTF-8 file contains the following UCS4 code points: \n";
    //     for (wchar_t c; fin >> c; )
    //         std::cout << "U+" << std::hex << std::setw(4) << std::setfill('0') << c << '\n';
    
    // std::codecvt<char16_t, char, std::mbstate_t>;
    // std::wstring_convert<
    //     deletable_facet<std::codecvt<char16_t, char, std::mbstate_t>>, char16_t> conv16;
    // std::u16string str16 = conv16.from_bytes(data);
 
 //   std::cout << "The UTF-8 file contains the following 
    return EXIT_SUCCESS;
}
