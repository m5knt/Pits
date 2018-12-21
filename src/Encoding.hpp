/**
 * @brief 文字列エンコーディングユーティリティー
 * @author Yukio KANEDA
 * @file 
 */

#ifndef PITS_ENCODING_HPP_
#define PITS_ENCODING_HPP_

#include <cstdint>  // uint8_t
#include <array>
#include <utility>  // pair

/*
 *
 */

enum char8_t : std::uint8_t;

namespace Pits {

/// 文字列エンコーディングユーティリティー
inline namespace Encoding {

/*
 * char8_t   8ビットの符号無し整数 __STDC_UTF_8__  マクロ定義時に UTF8保証 (予定 C2x:n2231)
 * char16_t 16ビットの符号無し整数 __STDC_UTF_16__ マクロ定義時に UTF16保証 (C++11)
 * char32_t 32ビットの符号無し整数 __STDC_UTF_32__ マクロ定義時に UTF32保証 (C++11)
 * 
 * ユニコード文字は 10ffff まで
 * 
 * Name |Bytes|            Bits|   Ranges| Remarks
 * -----+-----+----------------+---------+------------------------
 * UTF32|    4|              32|ffff'ffff|
 * -----+-----+----------------+---------+------------------------
 * UTF16|    2|              16|    'ffff|d800-dfff サロゲートペア
 * UTF16|    4|              32|  1f'ffff|
 * -----+-----+----------------+---------+------------------------
 * UTF8 |    1|          7 =  7|       7f|ascii互換部
 * UTF8 |    2|        5+6 = 11|      3ff|
 * UTF8 |    3|      4+6+6 = 16|     ffff|
 * UTF8 |    4|    3+6+6+6 = 21|  1f'ffff|
 * -----+-----+----------------+---------+------------------------
 * UTF8 |    5|  2+6+6+6+6 = 26| 3ff'ffff|仕様から削除された
 * UTF8 |    6|1+6+6+6+6+6 = 31|7fff'ffff|仕様から削除された
 * 
 * 参考
 * - Unicode        ... https://ja.wikipedia.org/wiki/Unicode
 * - Blocks         ... https://ja.wikipedia.org/wiki/%E3%83%96%E3%83%AD%E3%83%83%E3%82%AF_(Unicode)
 * - Special Block  ... https://en.wikipedia.org/wiki/Specials_(Unicode_block)
 */

/// ユニコード変換失敗時の文字
constexpr auto UnicodeReplacementCharacter = static_cast<char32_t>(0xfffd);

/// ユニコード文字の最大値
constexpr auto UnicodeCharacterMax = static_cast<char32_t>(0x10ffff);

/**
 * @brief ライブラリビルド時における "" と L"" のエンコード種別
 * ビルド環境として利用が見込まれる種別のみ扱います
 */
enum class EncodingType : std::uint8_t {

    // マルチバイト系
    UTF8,       ///< UTF8    1 ～ 4 Bytes
    SJIS,       ///< SJIS    1 ～ 2 Bytes (Aka CP932, MS932, Windows31J)
    // ワイド系
    UTF16,      ///< UTF16 Native Endian
    UTF32,      ///< UTF32 Native Endian
    // その他
    OTHER,
};

/*
 *
 */

namespace EncodingImplement {

template <class MultiByteChara>
constexpr auto GetMultiByteKanjiEncodingType(const MultiByteChara* target) noexcept -> EncodingType
{
    // 以下4つはプログラムソースで表現できないはずなので評価しない
    // UTF16BE { 0x6f, 0x22, 0x5b, 0x57 };
    // UTF16LE { 0x22, 0x6f, 0x57, 0x5b };
    // UTF32BE { 0x00, 0x00, 0x6f, 0x22, 0x00, 0x00, 0x5b, 0x57 };
    // UTF32LE { 0x22, 0x6f, 0x00, 0x00, 0x57, 0x5b, 0x00, 0x00 };

    using cast = std::uint8_t;
    {
        // UTF8 1 ～ 4バイト
        constexpr std::uint8_t source[] = { 0xe6, 0xbc, 0xa2, 0xe5, 0xad, 0x97 };
        auto s = source;
        auto t = target;
        if (cast(*t) == *s && cast(*++t) == *++s && cast(*++t) == *++s && cast(*++t) == *++s) {
            return EncodingType::UTF8;
        }
    }
    {
        // SJIS 1 ～ 2バイト
        constexpr std::uint8_t source[] = { 0x8a, 0xbf, 0x8e, 0x9a};
        auto s = source;
        auto t = target;
        if (cast(*t) == *s && cast(*++t) == *++s && cast(*++t) == *++s && cast(*++t) == *++s) {
            return EncodingType::SJIS;
        }
    }
    return EncodingType::OTHER;
}

template <class WideChara>
constexpr auto GetWideKanjiEncodingType(const WideChara* target) noexcept -> EncodingType
{
    if (sizeof(*target) == sizeof(char16_t)) {

        constexpr char16_t source[] = { 0x6f22, 0x5b57 };
        auto t = target;
        auto s = source;
        using cast = decltype(*s);
        if (cast(*t) == *s && cast(*++t) == *++s) {
            return EncodingType::UTF16;
        }

    } else if (sizeof(*target) == sizeof(char32_t)) {

        constexpr char32_t source[] = { 0x6f22, 0x5b57 };
        auto s = source;
        auto t = target;
        using cast = decltype(*s);
        if (cast(*t) == *s && cast(*++t) == *++s) {
            return EncodingType::UTF32;
        }
    }
    return EncodingType::OTHER;
}

template <class Char>
struct Encoding;

template <>
struct Encoding<char> {
    static constexpr auto GetEncodingType = GetMultiByteKanjiEncodingType<char>;
};

template <>
struct Encoding<char8_t> {
    static constexpr auto GetEncodingType = GetMultiByteKanjiEncodingType<char>;
};

template <>
struct Encoding<wchar_t> {
    static constexpr auto GetEncodingType = GetWideKanjiEncodingType<wchar_t>;
};

template <>
struct Encoding<char16_t> {
    static constexpr auto GetEncodingType = GetWideKanjiEncodingType<char16_t>;
};

template <>
struct Encoding<char32_t> {
    static constexpr auto GetEncodingType = GetWideKanjiEncodingType<char32_t>;
};

} // namespace EncodingImplement

/*
 *
 */

/**
 * @brief X"漢字" リテラルのエンコード種別を返す
 * @param kanji "漢字" L"漢字" u8"漢字" u"漢字" U"漢字" リテラルを渡す
 * @return EncodingType 
 */
template <class Chara>
constexpr auto GetKanjiEncodingType(const Chara* kanji) noexcept -> EncodingType {
    return EncodingImplement::Encoding<Chara>::GetEncodingType(kanji);
}

/**
 * @brief UTF8 イテレータが示す位置を UTF32 文字へ一文字変換しイテレータを進める
 * ステートを持たない為 UTF8 の中途位置は扱わない
 * @param it 変換開始位置 次の位置を返す
 * @param end コンテナ終端位置 イテレータの forward 移動が安全であるなら正しくなくともよい
 * @return UTF32 文字 / 失敗時は UnicodeReplacementCharacter を返す
 */
template <class UTF8Iterator>
constexpr auto UTF8ToUTF32(UTF8Iterator it, UTF8Iterator end = UTF8Iterator()) noexcept
    -> std::pair<char32_t, UTF8Iterator>
{
    if (it == end) {
        return {UnicodeReplacementCharacter, it};
    }

    // アスキーもしくはマルチバイト開始データの確認
    auto c = char32_t(*it++ & 0xff);
    auto remain = int{};

    if (c <= 0x7f) {
        return {c, it};
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
        return {UnicodeReplacementCharacter, it};
    }

    // マルチバイト文字
    for (; remain; --remain) {
        if (it == end) {
            return {UnicodeReplacementCharacter, it};
        }
        else if ((*it & 0b0'1100'0000) == 0b0'1000'0000) {
            c = (c << 6) | (*it++ & 0b0'0011'1111);
        } else {
            return {UnicodeReplacementCharacter, it};
        }
    }
    if (c > UnicodeCharacterMax) {
        return {UnicodeReplacementCharacter, it};
    }

    return {c, it};
}

/**
 * @brief UTF32 文字を UTF8 へ変換する
 * @param c UTF32 文字
 * @return [0] 常にデータあり / [1:3] != 0 ならデータ有り (変換失敗は UnicodeReplacementCharacter)
 */
constexpr auto UTF32ToUTF8(char32_t c) -> std::array<char, 4>
{
    if (c <= 0x7f) {
        return {char(c)};
    }
    else if (c <= 0x3ff) {
        auto c1 = char((c & 0b0'0011'1111) | 0b0'1000'0000); c >>= 6;
        auto c0 = char((c & 0b0'0011'1111) | 0b0'1100'0000);
        return {c0, c1};
    }
    else if (c <= 0xffff) {
        auto c2 = char((c & 0b0'0011'1111) | 0b0'1000'0000); c >>= 6;
        auto c1 = char((c & 0b0'0011'1111) | 0b0'1000'0000); c >>= 6;
        auto c0 = char((c & 0b0'0001'1111) | 0b0'1110'0000);
        return {c0, c1, c2};
    }
    else if (c <= UnicodeCharacterMax) {
        auto c3 = char((c & 0b0'0011'1111) | 0b0'1000'0000); c >>= 6;
        auto c2 = char((c & 0b0'0011'1111) | 0b0'1000'0000); c >>= 6;
        auto c1 = char((c & 0b0'0011'1111) | 0b0'1000'0000); c >>= 6;
        auto c0 = char((c & 0b0'0000'1111) | 0b0'1111'0000);
        return {c0, c1, c2, c3};
    }

    c = UnicodeReplacementCharacter;
    auto c2 = char((c & 0b0'0011'1111) | 0b0'1000'0000); c >>= 6;
    auto c1 = char((c & 0b0'0011'1111) | 0b0'1000'0000); c >>= 6;
    auto c0 = char((c & 0b0'0001'1111) | 0b0'1110'0000);
    return {c0, c1, c2};
}

/**
 * @brief サロゲートコードで有るか返す
 * @param c 文字
 * @return bool 真偽
 */
constexpr auto IsSurrogate(char16_t c) noexcept -> bool
{
    return (0xd800 <= c) && (c <= 0xdfff);
}

/**
 * @brief ハイサロゲートコードで有るか返す
 * @param c 文字
 * @return bool 真偽
 */
constexpr auto IsHighSurrogate(char16_t c) noexcept -> bool
{
    return (0xd800 <= c) && (c <= 0xdbff);
}

/**
 * @brief ローサロゲートコードで有るか返す
 * @param c 文字
 * @return bool 真偽
 */
constexpr auto IsLowSurrogate(char16_t c) noexcept -> bool
{
    return (0xdc00 <= c) && (c <= 0xdfff);
}

/**
 * @brief UTF16 イテレータが示す位置を UTF32 文字へ一文字変換しイテレータを進める
 * ステートを持たない為 UTF16 文字の中途位置は扱わない
 * @param it 変換開始位置 次の位置を返す 
 * @param end コンテナ終端位置 イテレータの forward 移動が安全であるなら正しくなくともよい
 * @return UTF32 文字 / 失敗時は UnicodeReplacementCharacter を返す
 */
template <class UTF16Iterator>
constexpr auto UTF16ToUTF32(UTF16Iterator it, UTF16Iterator end = UTF16Iterator()) noexcept
    -> std::pair<char32_t, UTF16Iterator>
{
    if (it == end) {
        return {UnicodeReplacementCharacter, it};
    }

    auto c = char32_t(*it++ & 0xffff);
    if (!IsSurrogate(c)) {
        return {c, it};
    }
    else if (IsHighSurrogate(c)) {
        if ((it != end) && IsLowSurrogate(*it)) {
            auto l = *it++ & 0xffff;
            c = 0x10000 | ((c - 0xd800) * 0x400) | (l - 0xdc00);
            if (c > UnicodeCharacterMax) {
                return {UnicodeReplacementCharacter, it};
            }
            return {c, it};
        }
        return {UnicodeReplacementCharacter, it};
    }
    else {
        return {UnicodeReplacementCharacter, it};
    }
}

/**
 * @brief 正規な UTF32 文字を UTF16 へ変換する
 * @param c UTF32 文字
 * @return [0] 常にデータあり / [1] != 0 ならデータ有り (変換失敗は UnicodeReplacementCharacter)
 */
constexpr auto UTF32ToUTF16(char32_t c) noexcept -> std::array<char16_t, 2>
{
    if (c <= 0xffff) {
        return {char16_t(c)};
    }
    else if (c <= UnicodeCharacterMax) {
        // サロゲートペア
        auto h = char16_t((c - 0x10000) / 0x400 + 0xd800);
        auto l = char16_t((c - 0x10000) % 0x400 + 0xdc00);
        return {h, l};
    }
    return {char16_t(UnicodeReplacementCharacter)};
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
