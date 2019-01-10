#include "Pits/String.hpp"
#include <uchar.h>
#include <cassert>
#include <string_view>
#include <iostream>
#include <codecvt>

using namespace std::literals;

int main() {

#ifdef _MSC_VER
    static_assert(Pits::String::DetectKanjiEncoding("漢字") == Pits::String::SJIS);
    static_assert(Pits::String::DetectKanjiEncoding(L"漢字") == Pits::String::UTF16);
#endif

#if defined(__STDC_UTF_16__) && defined(__STDC_UTF_32__)
    std::cout << "defined __STDC_UTF_16__ __STDC_UTF_32__" << std::endl;
    static_assert(Pits::String::DetectKanjiEncoding(u"漢字") == Pits::String::UTF16);
    static_assert(Pits::String::DetectKanjiEncoding(U"漢字") == Pits::String::UTF32);
#endif

    // ビルド環境で変わるのでここでは識別出来たかだけ確認
    static_assert(Pits::String::DetectKanjiEncoding("漢字") != Pits::String::UNKNOWN);
    static_assert(Pits::String::DetectKanjiEncoding(L"漢字") != Pits::String::UNKNOWN);

    static_assert(Pits::String::DetectKanjiEncoding(u8"漢字") == Pits::String::UTF8);
    static_assert(Pits::String::DetectKanjiEncoding(u8"\u6f22\u5b57") == Pits::String::UTF8);
    static_assert(Pits::String::DetectKanjiEncoding(u8"\U00006f22\U00005b57") == Pits::String::UTF8);
    static_assert(Pits::String::DetectKanjiEncoding(u"\u6f22\u5b57") == Pits::String::UTF16);
    static_assert(Pits::String::DetectKanjiEncoding(u"\U00006f22\U00005b57") == Pits::String::UTF16);
    static_assert(Pits::String::DetectKanjiEncoding(U"\u6f22\u5b57") == Pits::String::UTF32);
    static_assert(Pits::String::DetectKanjiEncoding(U"\U00006f22\U00005b57") == Pits::String::UTF32);

    return EXIT_SUCCESS;
}
