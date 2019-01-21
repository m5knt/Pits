#include "Pits/Encoding.hpp"
#include "Pits/Timer.hpp"

#include <cassert>
#include <string_view>
#include <iostream>
#include <codecvt>
#include <vector>

using namespace std::literals;

template <class Job>
void Bench(Job job) {
    Pits::Timer begin;
    job();
    std::cout << begin.GetElapsed() <<std::endl;
}

constexpr auto DefinedNDEBUG =
#ifdef NDEBUG
true;
#else
false;
#endif

constexpr auto BenchTimes = (DefinedNDEBUG ? 100 : 1);

int main() {

#if defined(__STDC_UTF_16__) && defined(__STDC_UTF_32__)

    static_assert(Pits::Unicode::IsSurrogate(u"𐐷"[0]));
    static_assert(Pits::Unicode::IsSurrogate(u"𐐷"[1]));
    static_assert(Pits::Unicode::IsHighSurrogate(u"𐐷"[0]));
    static_assert(Pits::Unicode::IsLowSurrogate(u"𐐷"[1]));
    static_assert(Pits::Unicode::IsNotCharacter(U'\U0000fffe'));
    static_assert(Pits::Unicode::IsNotCharacter(U'\U0000ffff'));
    static_assert(Pits::Unicode::IsNotCharacter(U'\U0001ffff'));
    static_assert(Pits::Unicode::IsUnsafeCharacter(char32_t(0x0000d800)));
    static_assert(Pits::Unicode::IsUnsafeCharacter(char32_t(0x0000ffff)));
    static_assert(Pits::Unicode::IsUnsafeCharacter(char32_t(0x00110000)));

    static_assert(Pits::Unicode::IsLeadUnit(char8_t(0x00)));
    static_assert(Pits::Unicode::IsLeadUnit(char8_t(0xc0)));
    static_assert(Pits::Unicode::IsLeadUnit(char8_t(0xe0)));
    static_assert(Pits::Unicode::IsLeadUnit(char8_t(0xf0)));
    static_assert(Pits::Unicode::IsLeadUnit(char16_t(0xd800)));
    static_assert(Pits::Unicode::IsLeadUnit(char32_t(0x0000)));

    static_assert(Pits::Unicode::LeadToUnits(char8_t(0x00)) == 1);
    static_assert(Pits::Unicode::LeadToUnits(char8_t(0xc0)) == 2);
    static_assert(Pits::Unicode::LeadToUnits(char8_t(0xe0)) == 3);
    static_assert(Pits::Unicode::LeadToUnits(char8_t(0xf0)) == 4);
    static_assert(Pits::Unicode::LeadToUnits(char16_t(0x0000)) == 1);
    static_assert(Pits::Unicode::LeadToUnits(char16_t(0xd800)) == 2);
    static_assert(Pits::Unicode::LeadToUnits(char32_t(0x0000)) == 1);

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

        std::cout << "Bench Unicode Convert (0 ～ 10ffff) x " << BenchTimes << std::endl;

        std::vector<char8_t[Pits::Unicode::UTF32UnitsToUTF8Units()]> utf8(Pits::Unicode::CharacterMax);
        std::vector<char32_t[Pits::Unicode::UTF8UnitsToUTF32Units()]> utf32(Pits::Unicode::CharacterMax);
        std::cout << "ConvertUTF32ToUTF8: ";
        Bench([&] {
            for (int j = 0; j < BenchTimes; ++j) {
                for (int c = 0; c < Pits::Unicode::CharacterMax; ++c) {
                    Pits::Unicode::ConvertUTF32ToUTF8(&c, &utf8[c][0]);
                }
            }
        });
        std::cout << "ConvertUTF8ToUTF32: ";
        Bench([&] {
            for (int j = 0; j < BenchTimes; ++j) {
                for (int c = 0; c < Pits::Unicode::CharacterMax; ++c) {
                    Pits::Unicode::ConvertUTF8ToUTF32(&utf8[c][0], &utf32[c][0]);
                }
            }
        });
        for (int c = 0; c < Pits::Unicode::CharacterMax; ++c) {
            assert(utf32[c][0] == c);
        }
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
        std::vector<char16_t[Pits::Unicode::UTF32UnitsToUTF16Units()]> utf16(Pits::Unicode::CharacterMax);
        std::vector<char8_t[Pits::Unicode::UTF16UnitsToUTF8Units()]> utf8(Pits::Unicode::CharacterMax);
        std::vector<char32_t[Pits::Unicode::UTF16UnitsToUTF32Units()]> utf32(Pits::Unicode::CharacterMax);
        std::cout << "ConvertUTF32ToUTF16: ";
        Bench([&] {
            for (int j = 0; j < BenchTimes; ++j) {
                for (int c = 0; c < Pits::Unicode::CharacterMax; ++c) {
                    Pits::Unicode::ConvertUTF32ToUTF16(&c, &utf16[c][0]);
                }
            }
        });
        std::cout << "ConvertUTF16ToUTF32: ";
        Bench([&] {
            for (int j = 0; j < BenchTimes; ++j) {
                for (int c = 0; c < Pits::Unicode::CharacterMax; ++c) {
                    Pits::Unicode::ConvertUTF16ToUTF32(&utf16[c][0], &utf32[c][0]);
                }
            }
        });
        for (int c = 0; c < Pits::Unicode::CharacterMax; ++c) {
            assert(utf32[c][0] == c);
        }

        std::cout << "ConvertUTF16ToUTF8: ";
        Bench([&] {
            for (int j = 0; j < BenchTimes; ++j) {
                for (int c = 0; c < Pits::Unicode::CharacterMax; ++c) {
                    Pits::Unicode::ConvertUTF16ToUTF8(&utf16[c][0], &utf8[c][0]);
                }
            }
        });
        std::cout << "ConvertUTF8ToUTF16: ";
        char16_t tmp[Pits::Unicode::UTF8UnitsToUTF16Units()]{};
        Bench([&] {
            for (int j = 0; j < BenchTimes; ++j) {
                for (int c = 0; c < Pits::Unicode::CharacterMax; ++c) {
                    Pits::Unicode::ConvertUTF8ToUTF16(&utf8[c][0], &utf16[c][0]);
                }
            }
        });
        for (int c = 0; c < Pits::Unicode::CharacterMax; ++c) {
            auto cc = char32_t{};
            Pits::Unicode::ConvertUTF16ToUTF32(&utf16[c][0], &cc);
            assert(cc == c);
        }
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
