#include "Pits/StringLiteral.hpp"
#include <uchar.h>
#include <cassert>
#include <string_view>
#include <iostream>
#include <codecvt>

using namespace std::literals;

int main() {

#ifdef _MSC_VER
    static_assert(Pits::StringLiteral::DetectEncoding("漢字") == Pits::StringLiteral::SJIS);
    static_assert(Pits::StringLiteral::DetectEncoding(L"漢字") == Pits::StringLiteral::UTF16);
#endif

#if defined(__STDC_UTF_16__) && defined(__STDC_UTF_32__)
    std::cout << "defined __STDC_UTF_16__ __STDC_UTF_32__" << std::endl;
    static_assert(Pits::StringLiteral::DetectEncoding(u"漢字") == Pits::StringLiteral::UTF16);
    static_assert(Pits::StringLiteral::DetectEncoding(U"漢字") == Pits::StringLiteral::UTF32);
#endif

    // ビルド環境で変わるのでここでは識別出来たかだけ確認
    static_assert(Pits::StringLiteral::DetectEncoding("漢字") != Pits::StringLiteral::OTHER);
    static_assert(Pits::StringLiteral::DetectEncoding(L"漢字") != Pits::StringLiteral::OTHER);

    static_assert(Pits::StringLiteral::DetectEncoding(u8"漢字") == Pits::StringLiteral::UTF8);
    static_assert(Pits::StringLiteral::DetectEncoding(u8"\u6f22\u5b57") == Pits::StringLiteral::UTF8);
    static_assert(Pits::StringLiteral::DetectEncoding(u8"\U00006f22\U00005b57") == Pits::StringLiteral::UTF8);
    static_assert(Pits::StringLiteral::DetectEncoding(u"\u6f22\u5b57") == Pits::StringLiteral::UTF16);
    static_assert(Pits::StringLiteral::DetectEncoding(u"\U00006f22\U00005b57") == Pits::StringLiteral::UTF16);
    static_assert(Pits::StringLiteral::DetectEncoding(U"\u6f22\u5b57") == Pits::StringLiteral::UTF32);
    static_assert(Pits::StringLiteral::DetectEncoding(U"\U00006f22\U00005b57") == Pits::StringLiteral::UTF32);

    return EXIT_SUCCESS;
}
