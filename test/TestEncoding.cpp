#include "Pits/Encoding.hpp"
#include "Pits/Timer.hpp"
#include <uchar.h>
#include <cassert>
#include <string_view>
#include <iostream>
#include <codecvt>

using namespace std::literals;
#if __cplusplus <= 201703L
namespace std {
    using u8string = basic_string<char>;
}
#endif

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

    {
        constexpr auto a0 = Pits::EncodingUTF8ToUTF32(u8"𐐷漢字");
        static_assert(std::get<1>(a0) == U'𐐷');
        constexpr auto a1 = Pits::EncodingUTF8ToUTF32(std::get<0>(a0));
        static_assert(std::get<1>(a1) == U'漢');
        constexpr auto a2 = Pits::EncodingUTF8ToUTF32(std::get<0>(a1));
        static_assert(std::get<1>(a2) == U'字');
        static_assert(std::get<1>(Pits::EncodingUTF8ToUTF32("\xff\0")) == Pits::EncodingErrorIllegalSequence);
    }
    {
        constexpr auto a0 = Pits::EncodingUTF16ToUTF32(u"𐐷漢字");
        static_assert(std::get<1>(a0) == U'𐐷');
        constexpr auto a1 = Pits::EncodingUTF16ToUTF32(std::get<0>(a0));
        static_assert(std::get<1>(a1) == U'漢');
        constexpr auto a2 = Pits::EncodingUTF16ToUTF32(std::get<0>(a1));
        static_assert(std::get<1>(a2) == U'字');
        static_assert(std::get<1>(Pits::EncodingUTF16ToUTF32(u"\xd800\0")) == Pits::EncodingErrorIllegalSequence);
    }
    {
        auto sv = U"𐐷漢字😀"sv;
        std::u8string out;
        out.reserve(sv.length());
        Pits::EncodingUTF32ToUTF8(sv.begin(), sv.end(), std::inserter(out, out.end()));
        assert(out == u8"𐐷漢字😀");
    }
    {
        auto sv = U"𐐷漢字😀"sv;
        std::u16string out;
        out.reserve(sv.length());
        Pits::EncodingUTF32ToUTF16(sv.begin(), sv.end(), std::inserter(out, out.end()));
        assert(out == u"𐐷漢字😀");
    }
    {
        auto sv = u8"𐐷漢字😀"sv;
        std::u32string out;
        out.reserve(sv.length());
        Pits::EncodingUTF8ToUTF32(sv.begin(), sv.end(), std::inserter(out, out.end()));
        assert(out == U"𐐷漢字😀");
    }
    {
        auto sv = u"𐐷漢字😀"sv;
        std::u32string out;
        out.reserve(sv.length());
        Pits::EncodingUTF16ToUTF32(sv.begin(), sv.end(), std::inserter(out, out.end()));
        assert(out == U"𐐷漢字😀");
    }
    {
        auto sv = u"𐐷漢字😀"sv;
        std::u8string out;
        out.reserve(sv.length());
        Pits::EncodingUTF16ToUTF8(sv.begin(), sv.end(), std::inserter(out, out.end()));
        assert(out == u8"𐐷漢字😀");
    }
    {
        auto sv = u8"𐐷漢字😀"sv;
        std::u16string out;
        out.reserve(sv.length());
        Pits::EncodingUTF8ToUTF16(sv.begin(), sv.end(), std::inserter(out, out.end()));
        assert(out == u"𐐷漢字😀");
    }
    {
        auto sv = "\xC0\xAF\xE0\x80\xBF\xF0\x81\x82\x41"sv;
        // [2] c0 af        => 00 2F    => 002f => RC RC    非最短  2 => 2
        // [3] e0 80 bf     => 00 00 3F => 003f => RC RC RC 非最短  3 => 3
        // [4] f0 81 82 41x => 01 02 xx => 1080 => RC RC RC *最終+3にASCII不正シーケンス 4 => 3
        // [1] 41           => 41       => 41                      1 => 1
        std::u16string out;
        Pits::EncodingUTF8ToUTF16(sv.begin(), sv.end(), std::inserter(out, out.end()));
        assert(out == u"\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\u0041");
        //assert(out == u"\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\u0041");
    }
    {
        auto sv = "\xED\xA0\x80\xED\xBF\xBF\xED\xAF\x41"sv;
        // [3] ed a0 80  => D 20 00 => D800 => RC RC RC サロゲート    3 => 3
        // [3] ed bf bf  => D 3F 3F => DFFF => RC RC RC サロゲート    3 => 3
        // [3] ed af 41x => D 2F xx => DBC0 => RC RC    *中途+2にASCII不正シーケンス 3 => 2
        // [1] 41        => 0041 => 41                                            1 => 1
        std::u16string out;
        Pits::EncodingUTF8ToUTF16(sv.begin(), sv.end(), std::inserter(out, out.end()));
        assert(out == u"\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\u0041");
        //assert(out == u"\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\u0041");
    }
    {
        auto sv = "\xF4\x91\x92\x93\xFF\x41\x80\xBF\x42"sv;
        // [4] f4 91 92 93 => 7 11 12 13 => 1D1493 => RC RC RC RC 大きい        4 => 4
        // [-] ff          =>                      => RC          開始ではない   1 => 1
        // [1] 41                                  => 41                       1 => 1
        // [-] 80                                  => RC          開始ではない   1 => 1                  
        // [-] bf                                  => RC          不正シーケンス 1 => 1
        // [1] 42                                  => 42                       1 => 1
        std::u16string out;
        Pits::EncodingUTF8ToUTF16(sv.begin(), sv.end(), std::inserter(out, out.end()));
        assert(out == u"\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\u0041\uFFFD\uFFFD\u0042");
    }
    {
        auto sv = "\xE1\x80\xE2\xF0\x91\x92\xF1\xBF\x41"sv;
        // [3] e1 80 e2x    => 01 00 xx    =>  1000 => RC 中途+2に不正シーケンス 3 => 1
        // [3] e2 f0x       => 02 xx       =>  2000 => RC 中途+2に不正シーケンス 3 => 1
        // [4] f0 91 92 f1x => 00 11 12 xx => 11480 => RC 中途+3に不正シーケンス 4 => 1
        // [4] f1 bf 41x    => 01 3f xx xx => 7F000 => RC *中途+2にASCII不正シーケンス 4 => 1
        // [1] 41           => 41 1:1
        std::u16string out;
        Pits::EncodingUTF8ToUTF16(sv.begin(), sv.end(), std::inserter(out, out.end()));
        assert(out == u"\uFFFD\uFFFD\uFFFD\uFFFD\u0041");
    }
    {
        auto sv = "\xC0\xAF"sv;
        // [2] c0 af        => 00 2F    => 002f => RC RC    非最短  2 => 2
        std::u16string u16;
        std::u32string u32;
        Pits::EncodingUTF8ToUTF16(sv.begin(), sv.end(), std::inserter(u16, u16.end()));
        Pits::EncodingUTF8ToUTF32(sv.begin(), sv.end(), std::inserter(u32, u32.end()));
        assert(u16.size() <= Pits::Unicode::UTF8UnitsToUTF16Units(sv.size()));
        assert(u32.size() <= Pits::Unicode::UTF8UnitsToUTF32Units(sv.size()));
    }
    {
        auto sv = "\xE0\x80\xBF"sv;
        // [3] e0 80 bf     => 00 00 3F => 003f => RC RC RC 非最短  3 => 3
        std::u16string u16;
        std::u32string u32;
        Pits::EncodingUTF8ToUTF16(sv.begin(), sv.end(), std::inserter(u16, u16.end()));
        Pits::EncodingUTF8ToUTF32(sv.begin(), sv.end(), std::inserter(u32, u32.end()));
        assert(u16.size() <= Pits::Unicode::UTF8UnitsToUTF16Units(sv.size()));
        assert(u32.size() <= Pits::Unicode::UTF8UnitsToUTF32Units(sv.size()));
    }
    {
        auto sv = "\xf0\x81\x82\x83"sv;
        // [4] e0 81 82 83  => 01 02 03 => 1083 => RC RC RC RC 非最短  4 => 4
        std::u16string u16;
        std::u32string u32;
        Pits::EncodingUTF8ToUTF16(sv.begin(), sv.end(), std::inserter(u16, u16.end()));
        Pits::EncodingUTF8ToUTF32(sv.begin(), sv.end(), std::inserter(u32, u32.end()));
        assert(u16.size() <= Pits::Unicode::UTF8UnitsToUTF16Units(sv.size()));
        assert(u32.size() <= Pits::Unicode::UTF8UnitsToUTF32Units(sv.size()));
    }

#endif

    return 0;
}
