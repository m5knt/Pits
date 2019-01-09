#include "Pits/StringLiteral.hpp"
#include <uchar.h>
#include <cassert>
#include <string_view>
#include <iostream>
#include <codecvt>

using namespace std::literals;

int main() {

#ifdef _MSC_VER
    static_assert(Pits::StringLiteral::EncodingDetect("漢字") == Pits::StringLiteral::SJIS);
    static_assert(Pits::StringLiteral::EncodingDetect(L"漢字") == Pits::StringLiteral::UTF16);
#endif

    static_assert(Pits::StringLiteral::EncodingDetect(u"漢字") == Pits::StringLiteral::UTF16);
    static_assert(Pits::StringLiteral::EncodingDetect(U"漢字") == Pits::StringLiteral::UTF32);

    // ビルド環境で変わるのでここでは識別出来たかだけ確認
    static_assert(Pits::StringLiteral::EncodingDetect("漢字") != Pits::StringLiteral::OTHER);

    // ビルド環境で変わるのでここでは識別出来たかだけ確認
    static_assert(Pits::StringLiteral::EncodingDetect(L"漢字") != Pits::StringLiteral::OTHER);

    return EXIT_SUCCESS;
}
