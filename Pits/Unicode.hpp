/**
 * @brief ユニコード関係
 * @author Yukio KANEDA
 * @file 
 */

#ifndef PITS_UNICODE_HPP_
#define PITS_UNICODE_HPP_

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

} // namespace Unicode
} // namespace Pits

/*
 *
 */

#endif

/*
 *
 */
