﻿/**
 * @brief プログラムソース中の文字リテラルのエンコーディングを調べるユーティリティー
 * @author Yukio KANEDA
 * @file 
 */

#ifndef PITS_STRING_HPP_
#define PITS_STRING_HPP_

#include <uchar.h>  // __STDC_UTF_16__ __STDC_UTF_32__
#include <string>

#if __cplusplus <= 201703L
 /// for C++17 char8_t
enum char8_t : unsigned char;

#endif

/*
 *
 */

namespace Pits {

/**
 * @brief プログラムソース中の文字リテラルのエンコーディングを調べるユーティリティー
 */
namespace String {

/**
 * @brief ライブラリビルド時における文字列リテラルのエンコード種別
 * ビルド環境として利用が見込まれる種別のみ扱います
 */
enum EncodingTypes {

    // マルチバイト系
    UTF8,       // 1 ～ 4 Bytes
    SJIS,       // 1 ～ 2 Bytes (AKA CP932, MS932, Windows31J)
    // EUCJP,

    // ワイド系
    UTF16,      // Native
    UTF32,      // Native

    // その他
    UNKNOWN,    //
};

/**
 * @brief "漢字" のエンコード種別を返す
 * 
 * C/C++言語仕様上 char[] wchar_t[] char16_t[] char32_t[] は
 * 特定のエンコードではない為、厳密に判定出来るよう関数を用意している
 * u8"" の char8_t[] は UTF8 と決まっているので用意していない
 * 
 * @param kanji "漢字" L"漢字" u"漢字" U"漢字" を渡す
 * @return EncodingType
 * 
 * SJIS でコンパイルされないと問題の出るソース等で次の様にする
 * static_assert(Pits::String::DetectKanjiEncoding("漢字") == Pits::String::SJIS);
 * 
 * https://docs.microsoft.com/ja-jp/cpp/build/reference/source-charset-set-source-character-set?view=vs-2017
 */
constexpr auto DetectKanjiEncoding(const char* kanji) noexcept -> EncodingTypes
{
    // 実際はヌル文字含め最短となる5要素を評価する
    // 以下4つはプログラムソースで表現できないはずなので評価しない
    // UTF16BE {0x6f, 0x22, 0x5b, 0x57};
    // UTF16LE {0x22, 0x6f, 0x57, 0x5b};
    // UTF32BE {0x00, 0x00, 0x6f, 0x22, 0x00, 0x00, 0x5b, 0x57};
    // UTF32LE {0x22, 0x6f, 0x00, 0x00, 0x57, 0x5b, 0x00, 0x00};

    using cast = std::uint8_t;
    {
        std::uint8_t origin[] = {0xe6, 0xbc, 0xa2, 0xe5, 0xad, 0x97};
        auto o = origin;
        auto k = kanji;
        if (cast(*k) == *o && cast(*++k) == *++o && cast(*++k) == *++o && cast(*++k) == *++o && cast(*++k) == *++o) {
            return UTF8;
        }
    }
    {
        std::uint8_t origin[] = {0x8a, 0xbf, 0x8e, 0x9a};
        auto o = origin;
        auto k = kanji;
        if (cast(*k) == *o && cast(*++k) == *++o && cast(*++k) == *++o && cast(*++k) == *++o) {
            return SJIS;
        }
    }
    return UNKNOWN;
}

/**
 * @brief リテラル L"漢字" のエンコード種別を返す
 */
constexpr auto DetectKanjiEncoding(const wchar_t* kanji) noexcept -> EncodingTypes
{
    if (sizeof(*kanji) == sizeof(char16_t)) {

        using cast = char16_t;
        char16_t origin[] = {0x6f22, 0x5b57};
        auto o = origin;
        auto k = kanji;
        if (cast(*k) == *o && cast(*++k) == *++o) {
            return UTF16;
        }
    }
    else if (sizeof(*kanji) == sizeof(char32_t)) {

        using cast = char32_t;
        char32_t origin[] = {0x6f22, 0x5b57};
        auto o = origin;
        auto k = kanji;
        if (cast(*k) == *o && cast(*++k) == *++o) {
            return UTF32;
        }
    }
    return UNKNOWN;
}

/**
 * @brief リテラル u"漢字" のエンコード種別を返す
 * 
 * __STDC_UTF_16__ 判定で十分と思われるが一貫性の為に用意
 */
constexpr auto DetectKanjiEncoding(const char16_t* kanji) noexcept -> EncodingTypes
{
#ifdef __STDC_UTF_16__
    char16_t origin[] = {0x6f22, 0x5b57};
    auto o = origin;
    auto k = kanji;
    return (*k == *o && *++k == *++o) ? UTF16 : UNKNOWN;
#else
    return UNKNOWN;
#endif
}

/**
 * @brief リテラル U"漢字" のエンコード種別を返す
 * 
 * __STDC_UTF_32__ 判定で十分と思われるが一貫性の為に用意
 */
constexpr auto DetectKanjiEncoding(const char32_t* kanji) noexcept -> EncodingTypes
{
#ifdef __STDC_UTF_32__
    char32_t origin[] = {0x6f22, 0x5b57};
    auto o = origin;
    auto k = kanji;
    return (*k == *o && *++k == *++o) ? UTF32 : UNKNOWN;
#else
    return UNKNOWN;
#endif
}

/*
 *
 */

/// 文字列リテラルのエンコード
constexpr auto MultiByteEncoding = DetectKanjiEncoding("漢字");

/// 文字列リテラルのエンコード
constexpr auto WideCharEncoding = DetectKanjiEncoding(L"漢字");

/// 文字列リテラルのエンコード
constexpr auto Char16Encoding = DetectKanjiEncoding(u"漢字");

/// 文字列リテラルのエンコード
constexpr auto Char32Encoding = DetectKanjiEncoding(U"漢字");

/*
 *
 */

} // namespace String

/*
 *
 */

} // namespace Pits

/*
 *
 */

#endif

/*
 *
 */
