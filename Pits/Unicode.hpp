/**
 * @brief ユニコード関係
 * @author Yukio KANEDA
 * @file 
 */

#ifndef PITS_UNICODE_HPP_
#define PITS_UNICODE_HPP_

#include <iterator> // iterator_traits

/*
 *
 */

#if __cplusplus <= 201703L
/// for C++17 char8_t
enum char8_t : unsigned char;
#endif

/*
 *
 */

namespace Pits {

/**
 * @brief ユニコード関係
 * - Official       ... https://www.unicode.org/versions/
 */
namespace Unicode {

/*
 * ""   char 8ビットの符号無し整数
 *      C++98 ロケール依存
 * 
 * u8"" char 8ビットの符号無し整数
 *      C++11 UTF8保証
 *      C++20 char8_t
 * 
 * u""  char16_t 16ビットの符号無し整数
 *      C++11 __STDC_UTF_16__ マクロ定義時に UTF16保証 VS2017 は既定
 * 
 * U""  char32_t 32ビットの符号無し整数
 *      C++11 __STDC_UTF_32__ マクロ定義時に UTF32保証 VS2017 は既定
 * 
 * ユニコード文字は 10ffff まで
 * 
 * Name  |    Ranges | Size | Remarks
 * ------+-----------+------+--------------------------
 * UTF32 |   1f'ffff | 1(4) | 
 * ------+-----------+------+--------------------------
 * UTF16 |    0'ffff | 1(2) | 
 * UTF16 |   1f'ffff | 2(4) | 
 * ------+-----------+------+--------------------------
 * UTF8  |        7f | 1(1) |           7 =  7 
 * UTF8  |       7ff | 2(2) |         5+6 = 11
 * UTF8  |    0'ffff | 3(3) |       4+6+6 = 16
 * UTF8  |   1f'ffff | 4(4) |     3+6+6+6 = 21
 * ------+-----------+------+--------------------------
 * UTF8  |  3ff'ffff | 5(5) |   2+6+6+6+6 = 26 仕様削除
 * UTF8  | 7fff'ffff | 6(6) | 1+6+6+6+6+6 = 31 仕様削除
 * 
 * バイトオーダーマーク BOM
 * 
 * Name    | Bytes          | Remarks
 * --------+----------------+----------
 * UTF8    | ef, bb, bf     | u+feff
 * UTF16BE | fe, ff         |
 * UTF16LE | ff, fe         |
 * --------+----------------+----------
 * UTF32BE | 00, 00, fe, ff | 自動判別不可
 * UTF32LE | ff, fe, 00, 00 | 自動判別不可
 * --------+----------------+----------
 * 
 * 参考
 * - Unicode        ... https://ja.wikipedia.org/wiki/Unicode
 * - Blocks         ... https://ja.wikipedia.org/wiki/ブロック_(Unicode)
 * - Special Block  ... https://en.wikipedia.org/wiki/Specials_(Unicode_block)
 * - BOM            ... https://ja.wikipedia.org/wiki/バイトオーダーマーク
 * - Official       ... https://www.unicode.org/versions/
 * - Encoding       ... https://www.unicode.org/versions/Unicode11.0.0/ch03.pdf#G7404
 * ― Code Point     ... https://ja.wikipedia.org/wiki/符号点
 * 
 */

/**
 * @addtogroup ユニコードキャラクタ関連
 * @{
 */

/// バイトオーダーマーク
constexpr auto ByteOrderMark = char32_t(0xfeff);

// fff9 INTERLINEAR ANNOTATION ANCHOR
// fffa INTERLINEAR ANNOTATION SEPARATOR
// fffb INTERLINEAR ANNOTATION TERMINATOR
// fffc OBJECT REPLACEMENT CHARACTER

/// ユニコード変換失敗時の文字 REPLACEMENT CHARACTER
constexpr auto ReplacementCharacter = char32_t(0xfffd);

// fffe NOT A CHARACTER
// ffff NOT A CHARACTER

/// ユニコード文字の最大値
constexpr auto CharacterMax = char32_t(0x10ffff);

/**
 * @brief サロゲートコードで有るか返す (0xd800 ～ 0xdfff)
 * @param c 文字
 * @return 真偽
 */
constexpr auto IsSurrogate(char32_t cp) noexcept -> bool
{
    return (char32_t(0xd800) <= cp) && (cp <= char32_t(0xdfff));
}

/**
 * @brief ハイサロゲートコードで有るか返す (0xd800 ～ 0xdbff)
 * @param c 文字
 * @return bool 真偽
 */
constexpr auto IsHighSurrogate(char32_t cp) noexcept -> bool
{
    return (char32_t(0xd800) <= cp) && (cp <= char32_t(0xdbff));
}

/**
 * @brief ローサロゲートコードで有るか返す (0xdc00 ～ 0xdfff)
 * @param c 文字
 * @return bool 真偽
 */
constexpr auto IsLowSurrogate(char32_t cp) noexcept -> bool
{
    return (char32_t(0xdc00) <= cp) && (cp <= char32_t(0xdfff));
}

/**
 * @brief キャラクタとして扱わないコードであるか返す (0xfffe, 0xffff ... 0x10fffe, 0x10ffff)
 * @param c 文字
 * @return 真偽
 */
constexpr auto IsNotCharacter(char32_t cp) noexcept -> bool
{
    return (cp & char32_t(0xfffe)) == char32_t(0xfffe);
}

/**
 * @brief 危険なコードであるか返す
 * @param c 文字
 * @return bool 真偽
 */
constexpr auto IsUnsafeCharacter(char32_t cp) -> bool
{
    return (CharacterMax < cp) || IsSurrogate(cp) || IsNotCharacter(cp);
}

/**
 * @brief 安全なコードであるか返す
 * @param c 文字
 * @return bool 真偽
 */
constexpr auto IsSafeCharacter(char32_t cp) -> bool
{
    return !IsUnsafeCharacter(cp);
}

/**
 * @}
 */

/// UTF8 後続データであるか返す
constexpr auto IsFollowUnit(char8_t unit) noexcept -> bool
{
    return (unit & 0xc0) == 0x80;
}

/// UTF8 先頭データであるか返す
constexpr auto IsLeadUnit(char8_t unit) noexcept -> bool
{
    return !IsFollowUnit(unit);
}

/// UTF16 後続データであるか返す
constexpr auto IsFollowUnit(char16_t unit) noexcept -> bool
{
    return (unit & 0xfc00) == 0xdc00;
}

/// UTF16 先頭データであるか返す
constexpr auto IsLeadUnit(char16_t unit) noexcept -> bool
{
    return !IsFollowUnit(unit);
}

/// UTF32 後続データであるか
constexpr auto IsFollowUnit(char32_t unit) noexcept -> bool
{
    return false;
}

/// UTF32 先頭データであるか返す
constexpr auto IsLeadUnit(char32_t unit) noexcept -> bool
{
    return true;
}

/**
 * @brief 指定範囲のコードポイント数を返す
 * @param begin 開始位置
 * @param end 終了位置 
 * @return コードポイント数
 */
template <class UTFXXIterator,
    class = typename std::iterator_traits<UTFXXIterator>::value_type
>
auto Points(UTFXXIterator begin, UTFXXIterator end) noexcept -> std::size_t
{
    auto count = std::size_t {};
    for (auto it = begin; it != end; ++it) {
        if (IsFollowUnit(*it)) continue;
        ++count;
    }
    return count;
}

/**
 * @brief UTF8 先頭ユニットからユニット数を返す
 * @param lead 先頭ユニット
 * @return ユニット数
 */
constexpr auto LeadToUnits(char8_t lead) noexcept -> int
{
    auto c = lead;

    [[likely]]
    if (c <= 0b0'0111'1111) {
        // UTF8: 0x00 ～ 0x7f, Bits: 7 => Unicode: 0x00'0000 ～ 0x00'007f
        return 1;
    }
    else if (c <= 0b0'1101'1111) {
        // UTF8: 0xc0 ～ 0xdf, Bits: 5+6 => Unicode: 0x00'0080 ～ 0x00'07ff
        return 2;
    }
    else [[likely]] if (c <= 0b0'1110'1111) {
        // UTF8: 0xe0 ～ 0xef, Bits: 4+6+6 => Unicode: 0x00'0800 ～ 0x00'ffff
        return 3;
    }
    else {
        // UTF8: 0xf0 ～ 0xf7, Bits: 3+6+6+6 => Unicode: 0x01'0000 ～ 0x1f'ffff
        return 4;
    }
}

/**
 * @brief UTF16 先頭ユニットからユニット数を返す
 * @param lead 先頭ユニット
 * @return ユニット数
 */
constexpr auto LeadToUnits(char16_t lead) noexcept -> int
{
    auto c = lead;

    [[likely]]
    if (!(0xd800 <= c && c <= 0xdbff)) {
        // Unicode: 0x00'0000 ～ 0x00'ffff, Bits: 16
        return 1;
    }
    else {
        // Unicode: 0x01'0000 ～ 0x1f'ffff, Bits: 10+10+(1)
        return 2;
    }
}

/**
 * @brief UTF32 先頭ユニットからユニット数を返す
 * @param lead 先頭ユニット
 * @return ユニット数
 */
constexpr auto LeadToUnits(char32_t lead) noexcept -> int
{
    static_cast<void>(lead);
    return 1;
}

/**
 * @brief UTF32 から UTF8 変換時の最長ユニット数を返す
 * @param from UTF32 ユニット数
 * @return UTF8 ユニット数
 * 
 * Name  |  Ranges | UTF32 |  UTF8 | Ratio | Remarks
 * ------+---------+-------+-------+-------+---------
 * UTF32 |      7f |  1(4) |  1(1) |  1/1  |
 * UTF32 |     7ff |  1(4) |  2(2) |  2/1  |
 * UTF32 |  0'ffff |  1(4) |  3(3) |  3/1  |
 * UTF32 | 1f'ffff |  1(4) | *4(4) | *4/1  |
 * 
 */
constexpr auto UTF32UnitsToUTF8Units(std::size_t from = 1) -> std::size_t
{
    return from * 4;
}

/**
 * @brief UTF32 から UTF16 変換時の最長ユニット数を返す
 * @param from UTF32 ユニット数
 * @return UTF16 ユニット数
 * 
 * Name  |  Ranges | UTF32 | UTF16 | Ratio | Remarks
 * ------+---------+-------+-------+-------+---------
 * UTF32 |      7f |  1(4) |  1(2) |  1/1  |
 * UTF32 |     7ff |  1(4) |  1(2) |  1/1  |
 * UTF32 |  0'ffff |  1(4) |  1(2) |  1/1  |
 * UTF32 | 1f'ffff |  1(4) | *2(4) | *2/1  |
 * 
 */
constexpr auto UTF32UnitsToUTF16Units(std::size_t from = 1) -> std::size_t
{
    return from * 2;
}

/**
 * @brief UTF8 から UTF32 変換時の最長ユニット数を返す
 * @param from UTF8 ユニット数
 * @return UTF32 ユニット数
 * 
 * Name  |  Ranges |  UTF8 | UTF32 | Ratio | Remarks
 * ------+---------+-------+-------+-------+---------
 * UTF8  |      7f |  1(1) | *1(4) | *1/1  |
 * UTF8  |     7ff |  2(2) |  1(4) |  1/2  |
 * UTF8  |  0'ffff |  3(3) |  1(4) |  1/3  |
 * UTF8  | 1f'ffff |  4(4) |  1(4) |  1/4  |
 * 
 */
constexpr auto UTF8UnitsToUTF32Units(std::size_t from = 4) -> std::size_t
{
    return from;
}

/**
 * @brief UTF16 から UTF32 変換時の最長ユニット数を返す
 * @param from UTF16 ユニット数
 * @return UTF32 ユニット数
 * 
 * Name  |  Ranges | UTF16 | UTF32 | Ratio | Remarks
 * ------+---------+-------+-------+-------+---------
 * UTF16 |      7f |  1(2) | *1(4) | *1/1  |
 * UTF16 |     7ff |  1(2) |  1(4) |  1/1  |
 * UTF16 |  0'ffff |  1(2) |  1(4) |  1/1  |
 * UTF16 | 1f'ffff |  2(4) |  1(4) |  1/2  |
 */
constexpr auto UTF16UnitsToUTF32Units(std::size_t from = 2) -> std::size_t
{
    return from;
}

/**
 * @brief UTF16 から UTF8 変換時の最長ユニット数を返す
 * @param from UTF16 ユニット数
 * @return UTF8 ユニット数
 * 
 * Name  |  Ranges | UTF16 |  UTF8 | Ratio | Remarks
 * ------+---------+-------+-------+-------+---------
 * UTF16 |      7f |  1(2) |  1(1) |  1/1  |
 * UTF16 |     7ff |  1(2) |  2(2) |  2/1  |
 * UTF16 |  0'ffff |  1(2) | *3(3) | *3/1  |
 * UTF16 | 1f'ffff |  2(4) |  4(4) |  4/2  |
 * 
 */
constexpr auto UTF16UnitsToUTF8Units(std::size_t from = 2) -> std::size_t
{
    return from * 3;
}

/**
 * @brief UTF8 から UTF16 変換時の最長ユニット数を返す
 * @param from UTF8 ユニット数
 * @return UTF16 ユニット数
 * 
 * Name  |  Ranges |  UTF8 | UTF16 | Ratio | Remarks
 * ------+---------+-------+-------+-------+---------
 * UTF8  |      7f |  1(1) | *1(2) | *1/1  |
 * UTF8  |     7ff |  2(2) |  1(2) |  1/2  |
 * UTF8  |  0'ffff |  3(3) |  1(2) |  1/3  |
 * UTF8  | 1f'ffff |  4(4) |  2(4) |  2/4  |
 * 
 */
constexpr auto UTF8UnitsToUTF16Units(std::size_t from = 4) -> std::size_t
{
    return from;
}

/**
 * @brief UTF32 を UTF8 へ1文字変換する
 * @param from UTF32 コード 正しいエンコーディングである事 (1 動く)
 * @param to UTF8 出力イテレーター (1 ～ 4 動く)
 * @return 移動後の from と to
 */
template <class UTF32Iterator, class UTF8Iterator,
    class = typename std::iterator_traits<UTF32Iterator>::value_type,
    class = typename std::iterator_traits<UTF8Iterator>::value_type
>
constexpr auto ConvertUTF32ToUTF8(UTF32Iterator from, UTF8Iterator to)
    noexcept(noexcept(*to++ = char8_t(*from++)))
    -> std::pair<UTF32Iterator, UTF8Iterator>
{
    auto c = char32_t(*from++);

    [[likely]]
    if (c <= 0x7f) {
        // Unicode: 0x00'0000 ～ 0x00'007f => UTF8: 0x00 ～ 0x7f, Bits: 7
        *to++ = char8_t(c);
        return {from, to};
    }
    else if (c <= 0x7ff) {
        // Unicode: 0x00'0080 ～ 0x00'07ff => UTF8: 0xc0 ～ 0xdf, Bits: 5+6
        *to++ = char8_t((c >> 06) & 0b0'0001'1111 | 0b0'1100'0000); // Bits: 5
        *to++ = char8_t((c >> 00) & 0b0'0011'1111 | 0b0'1000'0000); // Bits: 6
        return {from, to};
    }
    else [[likely]] if (c <= 0xffff) {
        // Unicode: 0x00'0800 ～ 0x00'ffff => UTF8: 0xe0 ～ 0xef, Bits: 4+6+6
        *to++ = char8_t((c >> 12) & 0b0'0000'1111 | 0b0'1110'0000); // Bits: 4
        *to++ = char8_t((c >> 06) & 0b0'0011'1111 | 0b0'1000'0000); // Bits: 6
        *to++ = char8_t((c >> 00) & 0b0'0011'1111 | 0b0'1000'0000); // Bits: 6
        return {from, to};
    }
    else {
        // Unicode: 0x01'0000 ～ 0x1f'ffff => UTF8: 0xf0 ～ 0xf7, Bits: 3+6+6+6 
        *to++ = char8_t((c >> 18) & 0b0'0000'0111 | 0b0'1111'0000); // Bits: 3
        *to++ = char8_t((c >> 12) & 0b0'0011'1111 | 0b0'1000'0000); // Bits: 6
        *to++ = char8_t((c >> 06) & 0b0'0011'1111 | 0b0'1000'0000); // Bits: 6
        *to++ = char8_t((c >> 00) & 0b0'0011'1111 | 0b0'1000'0000); // Bits: 6
        return {from, to};
    }
}

/**
 * @brief UTF32 を UTF16 へ1文字変換する
 * @param from UTF32 入力イテレータ 正しいエンコーディングである事 (1 動く)
 * @param to UTF16 出力イテレータ (1 ～ 2 動く)
 * @return 移動後の from と to
 */
template <class UTF16Iterator, class UTF32Iterator,
    class = typename std::iterator_traits<UTF32Iterator>::value_type,
    class = typename std::iterator_traits<UTF16Iterator>::value_type
>
constexpr auto ConvertUTF32ToUTF16(UTF32Iterator from, UTF16Iterator to)
    noexcept(noexcept(*to++ = char16_t(*from++)))
    -> std::pair<UTF32Iterator, UTF16Iterator>
{
    auto c = char32_t(*from++);

    [[likely]]
    if ((c -= (0x10000)) >= 0) {
        // Unicode: 0x00'0000 ～ 0x00'ffff, Bits: 16
        *to++ = char16_t(c);
        return {from, to};
    }
    else {
        // Unicode: 0x01'0000 ～ 0x1f'ffff, Bits: 10+10+(1)
        *to++ = char16_t(c / 0x400 + 0xd800);   // Bits: 10
        *to++ = char16_t(c % 0x400 + 0xdc00);   // Bits: 10
        return {from, to};
    }
}

/**
 * @brief UTF8 を UTF32 へ1文字変換する
 * @param from UTF8 入力イテレータ 正しいエンコーディングである事 (1 ～ 4 動く)
 * @param to UTF32 出力イテレータ (1 動く)
 * @return 移動後の from と to
 */
template <class UTF8Iterator, class UTF32Iterator,
    class = typename std::iterator_traits<UTF8Iterator>::value_type,
    class = typename std::iterator_traits<UTF32Iterator>::value_type
>
constexpr auto ConvertUTF8ToUTF32(UTF8Iterator from, UTF32Iterator to)
    noexcept(noexcept(*to++ = char32_t(*from++)))
    -> std::pair<UTF8Iterator, UTF32Iterator>
{
    auto c = char32_t(char8_t(*from++));

    [[likely]]
    if (c <= 0b0'0111'1111) {
        // UTF8: 0x00 ～ 0x7f, Bits: 7 => Unicode: 0x00'0000 ～ 0x00'007f
        *to++ = c;
        return {from, to};
    }
    else if (c <= 0b0'1101'1111) {
        // UTF8: 0xc0 ～ 0xdf, Bits: 5+6 => Unicode: 0x00'0080 ～ 0x00'07ff
        *to++ = (c & 0b0'0001'1111) << 06 |
          (*from++ & 0b0'0011'1111);
        return {from, to};
    }
    else [[likely]] if (c <= 0b0'1110'1111) {
        // UTF8: 0xe0 ～ 0xef, Bits: 4+6+6 => Unicode: 0x00'0800 ～ 0x00'ffff
        *to++ = (c & 0b0'0000'1111) << 12 |
          (*from++ & 0b0'0011'1111) << 06 |
          (*from++ & 0b0'0011'1111) << 00;
        return {from, to};
    }
    else {
        // UTF8: 0xf0 ～ 0xf7, Bits: 3+6+6+6 => Unicode: 0x01'0000 ～ 0x1f'ffff
        *to++ = (c & 0b0'0000'0111) << 18 |
          (*from++ & 0b0'0011'1111) << 12 |
          (*from++ & 0b0'0011'1111) << 06 |
          (*from++ & 0b0'0011'1111) << 00;
        return {from, to};
    }
}

/**
 * @brief UTF16 を UTF32 へ1文字変換する
 * @param from UTF16 入力イテレータ 正しいエンコーディングである事 (1 ～ 2 動く)
 * @param to UTF32 出力イテレータ (1 動く)
 * @return 移動後の from と to
 */
template <class UTF16Iterator, class UTF32Iterator,
    class = typename std::iterator_traits<UTF16Iterator>::value_type,
    class = typename std::iterator_traits<UTF32Iterator>::value_type
>
constexpr auto ConvertUTF16ToUTF32(UTF16Iterator from, UTF32Iterator to)
    noexcept(noexcept(*to++ = char32_t(*from++)))
    -> std::pair<UTF16Iterator, UTF32Iterator>
{
    auto c = char32_t(*from++);

    [[likely]]
    if (!IsHighSurrogate(c)) {
        // Unicode: 0x00'0000 ～ 0x00'ffff, Bits: 16
        *to++ = c;
        return {from, to};
    }
    else {
        // Unicode: 0x01'0000 ～ 0x1f'ffff, Bits: 10+10+(1)
        *to++ = ((c - 0xd800 + (0x40)) * 0x400) + (*from++ - 0xdc00);
        return {from, to};
    }
}

/**
 * @brief UTF16 を UTF8 へ1文字変換する
 * @param from UTF16 入力イテレータ 正しいエンコーディングである事 (1 ～ 2 動く)
 * @param to UTF8 出力イテレータ (1 ～ 4 動く)
 * @return 移動後の from と to
 */
template <class UTF16Iterator, class UTF8Iterator,
    class = typename std::iterator_traits<UTF16Iterator>::value_type,
    class = typename std::iterator_traits<UTF8Iterator>::value_type
>
constexpr auto ConvertUTF16ToUTF8(UTF16Iterator from, UTF8Iterator to)
    noexcept(noexcept(*to++ = char8_t(*from++)))
    -> std::pair<UTF16Iterator, UTF8Iterator>
{
    auto c = char32_t(*from++);

    [[likely]]
    if (c <= 0x7f) {
        // Unicode: 0x00'0000 ～ 0x00'007f => UTF8: 0x00 ～ 0x7f, Bits: 7
        *to++ = char8_t(c); // Bits: 7
        return {from, to};
    }
    else if (c <= 0x7ff) {
        // Unicode: 0x00'0080 ～ 0x00'07ff => UTF8: 0xc0 ～ 0xdf, Bits: 5+6
        *to++ = char8_t((c >> 06) /* & 0b11111 */ | 0b0'1100'0000); // Bits: 5
        *to++ = char8_t((c >> 00) & 0b0'0011'1111 | 0b0'1000'0000); // Bits: 6
        return {from, to};
    }
    else [[likely]] if (!IsHighSurrogate(c)) {
        // Unicode: 0x00'0800 ～ 0x00'ffff => UTF8: 0xe0 ～ 0xef, Bits: 4+6+6
        *to++ = char8_t((c >> 12) /* & 0b01111 */ | 0b0'1110'0000); // Bits: 4
        *to++ = char8_t((c >> 06) & 0b0'0011'1111 | 0b0'1000'0000); // Bits: 6
        *to++ = char8_t((c >> 00) & 0b0'0011'1111 | 0b0'1000'0000); // Bits: 6
        return {from, to};
    }
    else {
        // Unicode: 0x01'0000 ～ 0x1f'ffff => UTF8: 0xf0 ～ 0xf7, Bits: 3+6+6+6
        c = (c - 0xd800 + 0x40) * 0x400 + (*from++ - 0xdc00);
        *to++ = char8_t((c >> 18) /* & 0b00111 */ | 0b0'1111'0000); // Bits: 3
        *to++ = char8_t((c >> 12) & 0b0'0011'1111 | 0b0'1000'0000); // Bits: 6
        *to++ = char8_t((c >> 06) & 0b0'0011'1111 | 0b0'1000'0000); // Bits: 6
        *to++ = char8_t((c >> 00) & 0b0'0011'1111 | 0b0'1000'0000); // Bits: 6
        return {from, to};
    }
}

/**
 * @brief UTF8 を UTF16 へ1文字変換する
 * @param from UTF8 入力イテレータ 正しいエンコーディングである事 (1 ～ 4 動く)
 * @param to UTF16 出力イテレータ (1 ～ 2 動く)
 * @return 移動後の from と to
 */
template <class UTF8Iterator, class UTF16Iterator,
    class = typename std::iterator_traits<UTF8Iterator>::value_type,
    class = typename std::iterator_traits<UTF16Iterator>::value_type
>
constexpr auto ConvertUTF8ToUTF16(UTF8Iterator from, UTF16Iterator to)
    noexcept(noexcept(*to++ = char16_t(*from++)))
    -> std::pair<UTF8Iterator, UTF16Iterator>
{
    auto c = char32_t(char8_t(*from++));

    [[likely]]
    if (c <= 0b0'0111'1111) {
        // UTF8: 0x00 ～ 0x7f, Bits: 7, Unicode: 0x00'0000 ～ 0x00'007f
        *to++ = char16_t(c);
        return {from, to};
    }
    else if (c <= 0b0'1101'1111) {
        // UTF8: 0xc0 ～ 0xdf, Bits: 5+6, Unicode: 0x00'0080 ～ 0x00'07ff
        /***/ c = (c & 0b0'0001'1111) << 6 |
            (*from++ & 0b0'0011'1111);
        *to++ = char16_t(c);
        return {from, to};
    }
    else [[likely]] if (c <= 0b0'1110'1111) {
        // UTF8: 0xe0 ～ 0xef, Bits: 4+6+6, Unicode: 0x00'0800 ～ 0x00'ffff
        /***/ c = (c & 0b0'0000'1111) << 12 |
            (*from++ & 0b0'0011'1111) << 06 |
            (*from++ & 0b0'0011'1111) << 00;
        *to++ = char16_t(c);
        return {from, to};
    }
    else {
        // UTF8: 0xf0 ～ 0xf7, Bits: 3+6+6+6, Unicode: 0x01'0000 ～ 0x1f'ffff
        /***/ c = (c & 0b0'0000'0111) << 18 |
            (*from++ & 0b0'0011'1111) << 12 |
            (*from++ & 0b0'0011'1111) << 06 |
            (*from++ & 0b0'0011'1111) << 00;
        *to++ = char16_t(c / 0x400 - (0x40) + 0xd800);  // Bits: 10
        *to++ = char16_t(c % 0x400 + 0xdc00);           // Bits: 10
        return {from, to};
    }
};

/*
 *
 */

} // namespace Unicode
} // namespace Pits

/*
 *
 */

#endif

/*
 *
 */
