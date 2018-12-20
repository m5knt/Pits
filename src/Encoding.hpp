/**
 * @brief 文字列エンコーディングユーティリティー
 * @author Yukio KANEDA
 * @file 
 */

#ifndef PITS_ENCODING_HPP_
#define PITS_ENCODING_HPP_

#include <cstdint>  // uint8_t
#include <tuple>

/*
 *
 */

namespace Pits {

/// 文字列エンコーディングユーティリティー
inline namespace Encoding {

/*
 * char8_t   8ビットの符号無し整数 __STDC_UTF_8__  マクロ定義時に UTF8保証 (予定 C2x:n2231)
 * char16_t 16ビットの符号付き整数 __STDC_UTF_16__ マクロ定義時に UTF16保証 (C++11)
 * char32_t 32ビットの符号付き整数 __STDC_UTF_32__ マクロ定義時に UTF32保証 (C++11)
 * 
 * ユニコード文字は 10ffff まで
 * Name |Bytes|            Bits|   Ranges| Remarks
 * -----+-----+----------------+---------+------------------------
 * UTF32|    4|              32|ffff'ffff|
 * UTF16|    2|              16|    'ffff|d800-dfff サロゲートペア
 * UTF16|    4|              32|  1f'ffff|
 * UTF8 |    1|          7 =  7|       7f|ascii互換部
 * UTF8 |    2|        5+6 = 11|      3ff|
 * UTF8 |    3|      4+6+6 = 16|     ffff|
 * UTF8 |    4|    3+6+6+6 = 21|  1f'ffff|
 * UTF8 |    5|  2+6+6+6+6 = 26| 3ff'ffff|仕様から削除された
 * UTF8 |    6|1+6+6+6+6+6 = 31|7fff'ffff|仕様から削除された
 */

/// エラー時の文字
constexpr auto UnicodeErrorCharacter = static_cast<char32_t>(-1);

/// ETC => ユニコード変換失敗時の文字
constexpr auto UnicodeReplacementCharacter = static_cast<char32_t>(0xfffd);

/// ユニコード文字の最大値
constexpr auto UnicodeMaxCharacter = static_cast<char32_t>(0x10ffff);

/**
 * @brief ライブラリビルド時における "" と L"" のエンコード種別
 * ビルド環境として利用が見込まれる種別のみ扱います
 */
enum class EncodingType : std::uint8_t {
    UTF8,       ///< MultiBite UTF8 
    UTF16LE,    ///< MultiBite UTF16LE 
    UTF16BE,    ///< MultiBite UTF16BE
    UTF32LE,    ///< MultiBite UTF32LE
    UTF32BE,    ///< MultiBite UTF32LE
    SJIS,       ///< MultiBite SJIS CP932, MS932, Windows31J
    UTF16,      ///< Native Endian UTF16
    UTF32,      ///< Native Endian UTF32
    OTHER,
};

/// UTF8 "漢字"
constexpr std::uint8_t UTF8KanjiBytes[] = { 0xe6, 0xbc, 0xa2, 0xe5, 0xad, 0x97 };

/// UTF16LE L"漢字"
constexpr std::uint8_t UTF16LEKanjiBytes[] = { 0x22, 0x6f, 0x57, 0x5b };

/// UTF16BE L"漢字"
constexpr std::uint8_t UTF16BEKanjiBytes[] = { 0x6f, 0x22, 0x5b, 0x57 };

/// UTF32LE L"漢字"
constexpr std::uint8_t UTF32LEKanjiBytes[] = { 0x22, 0x6f, 0x00, 0x00, 0x57, 0x5b, 0x00, 0x00 };

/// UTF32BE L"漢字"
constexpr std::uint8_t UTF32BEKanjiBytes[] = { 0x00, 0x00, 0x6f, 0x22, 0x00, 0x00, 0x5b, 0x57 };

/// SJIS "漢字"
constexpr std::uint8_t SJISKanjiBytes[] = { 0x8a, 0xbf, 0x8e, 0x9a};

/**
 * @brief 文字列のエンコード種別を返す
 * @param kanji 文字列 "漢字" を渡す
 * @return EncodingType 
 */
constexpr auto GetEncodingType(const char* kanji) noexcept -> EncodingType
{
    using c = std::uint8_t;
 
    if (auto k = kanji) {
        auto t = UTF8KanjiBytes;
        if ((c(k[0]) == t[0]) && (c(k[1]) == t[1]) && (c(k[2]) == t[2]) && (c(k[3]) == t[3])) {
            return EncodingType::UTF8;
        }
        t = UTF16LEKanjiBytes;
        if ((c(k[0]) == t[0]) && (c(k[1]) == t[1]) && (c(k[2]) == t[2]) && (c(k[3]) == t[3])) {
            return EncodingType::UTF16LE;
        }
        t = UTF16BEKanjiBytes;
        if ((c(k[0]) == t[0]) && (c(k[1]) == t[1]) && (c(k[2]) == t[2]) && (c(k[3]) == t[3])) {
            return EncodingType::UTF16BE;
        }
        t = UTF32LEKanjiBytes;
        if ((c(k[0]) == t[0]) && (c(k[1]) == t[1]) && (c(k[2]) == t[2]) && (c(k[3]) == t[3])) {
            return EncodingType::UTF32LE;
        }
        t = UTF32BEKanjiBytes;
        if ((c(k[0]) == t[0]) && (c(k[1]) == t[1]) && (c(k[2]) == t[2]) && (c(k[3]) == t[3])) {
            return EncodingType::UTF32BE;
        }
        t = SJISKanjiBytes;
        if ((c(k[0]) == t[0]) && (c(k[1]) == t[1]) && (c(k[2]) == t[2]) && (c(k[3]) == t[3])) {
            return EncodingType::SJIS;
        }
    }
    return EncodingType::OTHER;
}

/// UTF16 L"漢字"
constexpr char16_t UTF16Kanji[] = { 0x6f22, 0x5b57 };

/// UTF32 L"漢字"
constexpr char32_t UTF32Kanji[] = { 0x6f22, 0x5b57 };

/**
 * @brief ワイド文字列のエンコード種別を返す
 * @param kanji 文字列 L"漢字" を渡す
 * @return EncodingType 
 * @note Windows は UTF16, Linux/OSX は UTF-32 が一般的である
 */
template <class Char>
constexpr auto GetEncodingType(const Char* kanji) noexcept -> EncodingType
{
    using c = unsigned int;
    if (auto k = kanji) {
        {
            auto u = UTF16Kanji;
            if ((sizeof(k[0]) == sizeof(u[0])) && (c(k[0]) == u[0]) && (c(k[1]) == u[1])) {
                return EncodingType::UTF16;
            }
        }
        {
            auto u = UTF32Kanji;
            if ((sizeof(k[0]) == sizeof(u[0])) && (c(k[0]) == u[0]) && (c(k[1]) == u[1])) {
                return EncodingType::UTF32;
            }
        }
    }
    return EncodingType::OTHER;
}

/**
 * @brief サロゲートコードで有るか返す
 * @param c 文字
 * @return bool 真偽
 */
constexpr auto IsSurrogate(int c) noexcept -> bool
{
    return (0xd800 <= c) && (c <= 0xdfff);
}

/**
 * @brief ハイサロゲートコードで有るか返す
 * @param c 文字
 * @return bool 真偽
 */
constexpr auto IsHighSurrogate(int c) noexcept -> bool
{
    return (0xd800 <= c) && (c <= 0xdbff);
}

/**
 * @brief ローサロゲートコードで有るか返す
 * @param c 文字
 * @return bool 真偽
 */
constexpr auto IsLowSurrogate(int c) noexcept -> bool
{
    return (0xdc00 <= c) && (c <= 0xdfff);
}

/**
 * @brief UTF8 イテレータが示す位置の UTF8 文字を UTF32 文字へ一文字変換しイテレータを進める
 * @param it 変換開始位置 変換後の位置を返す
 * @param end コンテナ終端位置
 * @return UTF32 文字 (失敗時はUnicodeErrorCharactor)
 */
template <class UTF8Iterator>
auto UTF8ToUTF32(UTF8Iterator& it, UTF8Iterator end) noexcept -> char32_t
{
    if (it == end) {
        return UnicodeErrorCharacter;
    }

    // アスキーもしくはマルチバイト開始データの確認
    auto c = *it++ & 0xff;
    auto remain = int{};

    if (c <= 0x7f) {
        return c;           //       7      7f ascii
    }
    else if ((c & 0b0'1110'0000) == 0b0'1100'0000) {
        c &= 0b0'0001'1111; //     5+6     3ff
        remain = 1;
    }
    else if ((c & 0b0'1111'0000) == 0b0'1110'0000) {
        c &= 0b0'0000'1111; //   4+6+6    ffff
        remain = 2;
    }
    else if ((c & 0b0'1111'1000) == 0b0'1111'0000) {
        c &= 0b0'0000'0111; // 3+6+6+6 1f'ffff
        remain = 3;
    }
    else {
        return UnicodeErrorCharacter;
    }

    // マルチバイト文字
    for (; remain; --remain) {
        if (it == end) {
            return UnicodeErrorCharacter;
        }
        else if ((*it & 0b0'1100'0000) == 0b0'1000'0000) {
            c = (c << 6) | (*it++ & 0b0'0011'1111);
        } else {
            return UnicodeErrorCharacter;
        }
    }
    if (c > UnicodeMaxCharacter) {
        return UnicodeErrorCharacter;
    }

    return c;
}

/**
 * @brief UTF16 イテレータが示す位置の UTF16 文字を UTF32 文字へ一文字変換しイテレータを進める
 * @param it 変換開始位置 変換後の位置を返す
 * @param end コンテナ終端位置
 * @return UTF32 文字 (失敗時はUnicodeErrorCharactor)
 */
template <class UTF16Iterator>
auto UTF16ToUTF32(UTF16Iterator& it, UTF16Iterator end) noexcept -> char32_t
{
    if (it == end) {
        return UnicodeErrorCharacter;
    }

    auto c = *it++ & 0xffff;
    if (!IsSurrogate(c)) {
        return c;
    }
    else if (IsHighSurrogate(c)) {
        if ((it != end) && IsLowSurrogate(*it)) {
            auto l = *it++ & 0xffff;
            c = 0x10000 | ((c - 0xd800) * 0x400) | (l - 0xdc00);
            if (c > UnicodeMaxCharacter) {
                return UnicodeErrorCharacter;
            }
            return c;
        }
        return UnicodeErrorCharacter;
    }
    else {
        return UnicodeErrorCharacter;
    }
}

/**
 * @brief UTF32 文字を UTF16 文字へ変換する
 * 
 * @param c UTF32 文字
 * @return UTF16 文字のペア second が 0 なら1文字
 */
constexpr auto UTF32ToUTF16(char32_t c) -> std::pair<char16_t, char16_t>
{
   if (c <= 0xffff) return {char16_t(c), 0};
   auto h = char16_t((c - 0x10000) / 0x400 + 0xd800);
   auto l = char16_t((c - 0x10000) % 0x400 + 0xdc00);
   return {h, l};
}

/*
 *
 */

} // inline namespace Encoding
} // namespace Pits

/*
 *
 */

#endif

/*
 *
 */
