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

namespace Pits {

/**
 * @brief ユニコード関係
 * 資料
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
 * Name  |    Ranges | Size |  UTF8 | UTF16 | UTF32 | Remarks
 * ------+-----------+------+-------+-------+-------+-----------------------------
 * UTF32 |        7f | 1(4) |  1(1) |  1(2) | ----- |
 * UTF32 |       7ff | 1(4) |  2(2) |  1(2) | ----- |
 * UTF32 |    0'ffff | 1(4) |  3(3) |  1(2) | ----- |
 * UTF32 |   1f'ffff | 1(4) | *4(4) | *2(4) | ----- |
 * ------+-----------+------+-------+-------+-------+-----------------------------
 * UTF16 |        7f | 1(2) |  1(1) |  ---- | *1(4) |
 * UTF16 |       7ff | 1(2) |  2(2) |  ---- |  1(4) |
 * UTF16 |    0'ffff | 1(2) | *3(3) |  ---- |  1(4) |
 * UTF16 |   1f'ffff | 2(4) |  4(4) |  ---- |  1(4) |
 * ------+-----------+------+-------+-------+-------+-----------------------------
 * UTF8  |        7f | 1(1) |  ---- | *1(2) | *1(4) |           7 =  7 
 * UTF8  |       7ff | 2(2) |  ---- |  1(2) |  1(4) |         5+6 = 11
 * UTF8  |    0'ffff | 3(3) |  ---- |  1(2) |  1(4) |       4+6+6 = 16
 * UTF8  |   1f'ffff | 4(4) |  ---- |  2(4) |  1(4) |     3+6+6+6 = 21
 * ------+-----------+------+-------+-------+-------+-----------------------------
 * UTF8  |  3ff'ffff | 5(5) |  ---- |  ---- |  ---- |   2+6+6+6+6 = 26 仕様削除
 * UTF8  | 7fff'ffff | 5(5) |  ---- |  ---- |  ---- | 1+6+6+6+6+6 = 31 仕様削除
 * 
 * バイトオーダーマーク BOM
 * 
 * Name    | Bytes          | Remarks
 * --------+----------------+----------
 * UTF8    | ef, bb, bf     | fe ff
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
 */

/**
 * @addtogroup ユニコード関連定数
 * @{
 */

/// バイトオーダーマーク
constexpr char32_t ByteOrderMark = 0xfeff;

/// ユニコード変換失敗時の文字
constexpr char32_t ReplacementCharacter = 0xfffd;

/// ユニコード文字の最大値
constexpr char32_t CharacterMax = 0x10ffff;

/**
 * @}
 */

/**
 * @addtogroup ユニコードキャラクタ判定関数
 * @{
 */

/**
 * @brief キャラクタとして扱わないコードであるか返す (0xfffe, 0xffff ... 0x10fffe, 0x10ffff)
 * @param c 文字
 * @return 真偽
 */
constexpr auto IsNoncharacters(char32_t c) noexcept -> bool
{
    return (c & 0xfffe) == 0xfffe;
}

/**
 * @brief サロゲートコードで有るか返す (0xd800 ～ 0xdfff) 
 * @param c 文字
 * @return 真偽
 */
constexpr auto IsSurrogate(char32_t c) noexcept -> bool
{
    return (0xd800 <= c) && (c <= 0xdfff);
}

/**
 * @brief ハイサロゲートコードで有るか返す (0xd800 ～ 0xdbff) 
 * @param c 文字
 * @return bool 真偽
 */
constexpr auto IsHighSurrogate(char32_t c) noexcept -> bool
{
    return (0xd800 <= c) && (c <= 0xdbff);
}

/**
 * @brief ローサロゲートコードで有るか返す (0xdc00 ～ 0xdfff)
 * @param c 文字
 * @return bool 真偽
 */
constexpr auto IsLowSurrogate(char32_t c) noexcept -> bool
{
    return (0xdc00 <= c) && (c <= 0xdfff);
}

/**
 * @brief 危険なコードであるか返す
 * @param c 文字
 * @return bool 真偽
 */
constexpr auto IsUnsafeUTF32(char32_t c) -> bool
{
    return (CharacterMax < c) || IsSurrogate(c) || IsNoncharacters(c);
}

/**
 * @brief 安全なコードであるか返す
 * @param c 文字
 * @return bool 真偽
 */
constexpr auto IsSafeUTF32(char32_t c) -> bool
{
    return !IsUnsafeUTF32(c);    
}

/**
 * @}
 */

/**
 * @brief UTF32 を UTF8 へ一文字変換する
 * @param from UTF32 コード 正しいエンコーディングである事 (1 動く)
 * @param out UTF8 出力イテレーター (1 ～ 4 動く)
 * @return 移動したout
 */
template <class UTF32Iterator, class UTF8Iterator,
    class = typename std::iterator_traits<UTF32Iterator>::value_type,
    class = typename std::iterator_traits<UTF8Iterator>::value_type
>
constexpr auto ConvertUTF32ToUTF8(UTF32Iterator from, UTF8Iterator to) noexcept ->
    std::pair<UTF32Iterator, UTF8Iterator>
{
    auto c = char32_t(*from++);

    if (c <= 0x7f) {
        *to++ = char8_t(c);
    }
    else if (c <= 0x7ff) {
        *to++ = char8_t(((c >> 6) & 0b0'0011'1111) | 0b0'1100'0000);
        *to++ = char8_t(((c >> 0) & 0b0'0011'1111) | 0b0'1000'0000);
    }
    else if (c <= 0xffff) {
        *to++ = char8_t(((c >> 12) & 0b0'0001'1111) | 0b0'1110'0000);
        *to++ = char8_t(((c >> 6) & 0b0'0011'1111) | 0b0'1000'0000);
        *to++ = char8_t(((c >> 0) & 0b0'0011'1111) | 0b0'1000'0000);
    }
    else {
        *to++ = char8_t(((c >> 18) & 0b0'0000'1111) | 0b0'1111'0000);
        *to++ = char8_t(((c >> 12) & 0b0'0011'1111) | 0b0'1000'0000);
        *to++ = char8_t(((c >> 6) & 0b0'0011'1111) | 0b0'1000'0000);
        *to++ = char8_t(((c >> 0) & 0b0'0011'1111) | 0b0'1000'0000);
    }

    return {from, to};
}

/**
 * @brief UTF32 を UTF16 へ一文字変換する
 * @param from UTF32 入力イテレータ 正しいエンコーディングである事 (1 動く)
 * @param to UTF16 出力イテレータ (1 ～ 2 動く)
 * @return 移動後の from と to
 */
template <class UTF16Iterator, class UTF32Iterator,
    class Require0 = typename std::iterator_traits<UTF32Iterator>::value_type,
    class Require1 = typename std::iterator_traits<UTF16Iterator>::value_type
>
constexpr auto ConvertUTF32ToUTF16(UTF32Iterator from, UTF16Iterator to) noexcept
    -> std::pair<UTF32Iterator, UTF16Iterator>
{
    auto c = char32_t(*from++);

    if (c <= 0xffff) {
        *to++ = char16_t(c);
    }
    else {
        *to++ = char16_t(((c - 0x10000) / 0x400) + 0xd800);
        *to++ = char16_t(((c - 0x10000) % 0x400) + 0xdc00);
    }

    return {from, to};
}

/**
 * @brief UTF8 を UTF32 へ一文字変換する
 * @param from UTF8 入力イテレータ 正しいエンコーディングである事 (1 ～ 4 動く)
 * @param to UTF32 出力イテレータ (1 動く)
 * @return 移動後の from と to
 */
template <class UTF8Iterator, class UTF32Iterator,
    class Require0 = typename std::iterator_traits<UTF8Iterator>::value_type,
    class Require1 = typename std::iterator_traits<UTF32Iterator>::value_type
>
constexpr auto ConvertUTF8ToUTF32(UTF8Iterator from, UTF32Iterator to) noexcept
    -> std::pair<UTF8Iterator, UTF32Iterator>
{
    auto c = char32_t(*from++ & 0xff);

    /**/ if (c < 0b0'1000'0000) {   //  0 ～ 7f 0 ～ 7f 7
        static_cast<void>(nullptr);
    }
    else if (c < 0b0'1110'0000) {   // c0 ～ df 80 ～ 7ff 5+6 
        c = (c & 0b0'0001'1111) << 6;
        c = c | (*from++ & 0b0'0011'1111);
    }
    else if (c < 0b0'1111'0000) {   // e0 ～ ef 800 ～ ffff　4+6+6 
        c = (c & 0b0'0000'1111) << 12;
        c = c | ((*from++ & 0b0'0011'1111) << 6);
        c = c | ((*from++ & 0b0'0011'1111) << 0);
    }
    else if (c < 0b0'1111'1000) {   // f0 ～ f7 1'0000 ～ 1f'ffff 3+6+6+6 
        c = (c & 0b0'0000'0111) << 18;
        c = c | ((*from++ & 0b0'0011'1111) << 12);
        c = c | ((*from++ & 0b0'0011'1111) << 6);
        c = c | ((*from++ & 0b0'0011'1111) << 0);
    }

    *to++ = c;
    return {from, to};
}

/**
 * @brief UTF16 を UTF32 へ一文字変換する
 * @param from UTF16 入力イテレータ 正しいエンコーディングである事 (1 ～ 2 動く)
 * @param to UTF32 出力イテレータ (1 動く)
 * @return 移動後の from と to
 */
template <class UTF16Iterator, class UTF32Iterator,
    class Require0 = typename std::iterator_traits<UTF16Iterator>::value_type,
    class Require1 = typename std::iterator_traits<UTF32Iterator>::value_type
>
constexpr auto ConvertUTF16ToUTF32(UTF16Iterator from, UTF32Iterator to) noexcept
    -> std::pair<UTF16Iterator, UTF32Iterator>
{
    auto c = char32_t(*from++);

    if ((0xd800 <= c) && (c <= 0xdbff)) {
        c = 0x10000 | ((c - 0xd800) * 0x400) | (*from++ & 0x3ff);
    }

    *to++ = c;
    return {from, to};
}

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
