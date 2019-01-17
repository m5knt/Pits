/**
 * @brief 文字列エンコーディング関係
 * @author Yukio KANEDA
 * @file 
 */

#ifndef PITS_ENCODING_HPP_
#define PITS_ENCODING_HPP_

#include <cstdint>  // uint8_t
#include <uchar.h>  // __STDC_UTF_16__ __STDC_UTF_32__
#include <array>
#include <utility>  // pair
#include <iterator> // iterator_traits

#include "Pits/Unicode.hpp"

/*
 *
 */

#if __cplusplus <= 201703L
using char8_t = char;
namespace std {
    using u8string = std::string;
}
#endif

/*
 *
 */

namespace Pits {

/**
 * @brief 文字列エンコーディング関係
 */
inline namespace Encoding {

/*
 *
 */

/// 不正なデータ列 (UTF32と被らない値)
constexpr char32_t EncodingErrorIllegalSequence = char32_t(-1);

/// エンコーディングに必要なデータが足りない (UTF32と被らない値)
constexpr char32_t EncodingErrorNotEnough = char32_t(-2);

/**
 * @brief エンコーディングエラーなら文字を置き換える
 * @param from 文字
 * @return 文字
 */
constexpr auto ReplacementIfEncodingError(char32_t from) -> char32_t
{
    return from <= Unicode::CharacterMax ? from : Unicode::ReplacementCharacter;
}

/**
 * @brief UTF32 を UTF8 に変換する
 *
 * @param begin 読み込み開始位置
 * @param end 読み込み終了位置
 * @param to 出力位置
 *
 * @return 移動後の begin, to
 */
template <class UTF32InIter, class UTF8Inserter,
    class = typename std::iterator_traits<UTF32InIter>::value_type,
    class = typename std::iterator_traits<UTF8Inserter>::value_type
>
constexpr auto EncodingUTF32ToUTF8(UTF32InIter begin, UTF32InIter end, UTF8Inserter to)
    noexcept(noexcept(*to++ = char8_t(*begin++)))
    -> std::pair<UTF32InIter, UTF8Inserter>
{
    auto it = begin;
    while (it != end) {
        auto c = char32_t(*it++);
        auto safe = Unicode::IsSafeCharacter(c) ? c : Unicode::ReplacementCharacter;

        // UTF8 化
        auto to8 = Unicode::ConvertUTF32ToUTF8(&safe, to);
        to = std::get<1>(to8);
    }
    return {it, to};
}

/**
 * @brief UTF32 を UTF16 に変換する
 *
 * @param begin 読み込み開始位置
 * @param end 読み込み終了位置
 * @param to 出力位置
 *
 * @return 移動後の begin, to
 */
template <class UTF32InIter, class UTF16Inserter,
    class = typename std::iterator_traits<UTF32InIter>::value_type,
    class = typename std::iterator_traits<UTF16Inserter>::value_type
>
constexpr auto EncodingUTF32ToUTF16(UTF32InIter begin, UTF32InIter end, UTF16Inserter to)
    noexcept(noexcept(*to++ = char16_t(*begin++)))
    -> std::pair<UTF32InIter, UTF16Inserter>
{
    auto it = begin;
    while (it != end) {
        auto c = char32_t(*it++);
        auto safe = Unicode::IsSafeCharacter(c) ? c : Unicode::ReplacementCharacter;

        // UTF16 化
        auto to16 = Unicode::ConvertUTF32ToUTF16(&safe, to);
        to = std::get<1>(to16);
    }
    return {it, to};
}

/**
 * @brief イテレータが示す位置を UTF8 から UTF32 へ一文字変換する Unicode11準拠
 *
 * ステートを持たない為、中途位置はエラー扱いになる
 * 
 * @param it 変換開始位置 end 以外の位置である事
 * @param end コンテナ終端位置 イテレータの forward 移動が安全であるなら正しくなくともよい
 * 
 * @return 文字, 移動後イテレータ をペアで返す
 */
template <class UTF8Iterator,
    class = typename std::iterator_traits<UTF8Iterator>::value_type
>
constexpr auto EncodingUTF8ToUTF32(UTF8Iterator begin, UTF8Iterator end = UTF8Iterator())
    noexcept(noexcept(*begin++))
    -> std::pair<UTF8Iterator, char32_t>
{
    auto it = begin;

    // シングルコードの確認
    auto c = char32_t(*it++ & 0xff);
    if (c < 0b0'1000'0000) {        //  0 ～ 7f 0 ～ 7f 7
        return {it, c};
    }

    auto min = char32_t {};
    auto req = int {};

    /**/ if (c < 0b0'1100'0000) {   // 80 ～ bf
        // 読み込みコードポイント毎に置き換え
        return {it, EncodingErrorIllegalSequence};
    }
    else if (c < 0b0'1110'0000) {   // c0 ～ df 80 ～ 7ff 5+6 
        c &= 0b0'0001'1111;
        min = 0x80;
        req = 1;
    }
    else if (c < 0b0'1111'0000) {   // e0 ～ ef 800 ～ ffff　4+6+6 
        c &= 0b0'0000'1111;
        min = 0x800;
        req = 2;
    }
    else if (c < 0b0'1111'1000) {   // f0 ～ f7 1'0000 ～ 1f'ffff 3+6+6+6 
        c &= 0b0'0000'0111;
        min = 0x10000;
        req = 3;
    }
    else {
        // 読み込みコードポイント毎に置き換え
        return {it, EncodingErrorIllegalSequence};
    }

    // マルチバイト後続確認
    for (; req; --req) {

        // 中途で終了か
        if (it == end) {
            return {++begin, EncodingErrorNotEnough};
        }

        // 後続データか
        auto t = char32_t(*it & 0xff);
        if (0b0'1000'0000 <= t && t < 0b0'1100'0000) {   // 80 ～ bf
            c = (c << 6) | (t & 0b0'0011'1111);
            ++it;
        }
        else {
            // コード評価で置き換え数が変わるので値を計算
            for (; req; --req) c <<= 6;

            // 不正コードなら読み込みコードポイント毎に置き換え
            if (c < min || Unicode::IsUnsafeCharacter(c)) {
                return {++begin, EncodingErrorIllegalSequence};
            }

            // 進めたコードポイント範囲を置き換え
            return {it, EncodingErrorIllegalSequence};
        }
    }

    // 不正コードなら読み込みコードポイント毎に置き換え
    if (c < min || Unicode::IsUnsafeCharacter(c)) {
        return {++begin, EncodingErrorIllegalSequence};
    }

    // 成功
    return {it, c};
}

/**
 * @brief UTF8 を UTF32 に変換する
 *
 * @param begin 読み込み開始位置
 * @param end 読み込み終了位置
 * @param to 出力位置
 *
 * @return 移動後の begin, to
 */
template <class UTF8Iterator, class UTF32Inserter,
    class = typename std::iterator_traits<UTF8Iterator>::value_type,
    class = typename std::iterator_traits<UTF32Inserter>::value_type
>
constexpr auto EncodingUTF8ToUTF32(UTF8Iterator begin, UTF8Iterator end, UTF32Inserter to)
    noexcept(noexcept(*to++ = char32_t(*begin++)))
    -> std::pair<UTF8Iterator, UTF32Inserter>
{
    auto it = begin;
    while (it != end) {
        auto to32 = EncodingUTF8ToUTF32(it, end);

        // シーケンス中途で end なら終える
        auto c = std::get<1>(to32);
        if (c == EncodingErrorNotEnough) break;
        it = std::get<0>(to32);
        auto safe = ReplacementIfEncodingError(c);

        *to++ = safe;
    }
    return {it, to};
}

/**
 * @brief イテレータが示す位置を UTF16 から UTF32 へ一文字変換する
 * 
 * ステートを持たない為、中途位置はエラー扱いになる
 * 
 * @param it 変換開始位置 end 以外の位置である事
 * @param end コンテナ終端位置 イテレータの forward 移動が安全であるなら正しくなくともよい
 * 
 * @return 文字, 移動後イテレータ をペアで返す
 *      文字 <= Unicode::CharacterMax ... 成功
 *      文字 > Unicode::CharacterMax ... エラー
 */
template <class UTF16Iterator,
    class = typename std::iterator_traits<UTF16Iterator>::value_type
>
constexpr auto EncodingUTF16ToUTF32(UTF16Iterator begin, UTF16Iterator end = UTF16Iterator())
    noexcept(noexcept(*begin++))
    -> std::pair<UTF16Iterator, char32_t>
{
    auto it = begin;

    auto c = char32_t(*it++ & 0xffff);

    // ハイサロゲートか
    if (Unicode::IsHighSurrogate(c)) {

        if (it == end) {
            return {it, EncodingErrorNotEnough};
        }

        // ローサロゲートか
        auto l = char32_t(*it & 0xffff);
        if (Unicode::IsLowSurrogate(l)) {
            c = 0x10000 + ((c - 0xd800) * 0x400) + (l - 0xdc00);
            ++it;
        }
        else {
            // 進めたコードポイント範囲を置き換え
            return {it, EncodingErrorIllegalSequence};
        }
    }

    // 不正コードなら置き換え
    if (Unicode::IsUnsafeCharacter(c)) {
        return {it, EncodingErrorIllegalSequence};
    }

    // 成功
    return {it, c};
}

/**
 * @brief UTF16 を UTF32 に変換する
 *
 * @param begin 読み込み開始位置
 * @param end 読み込み終了位置
 * @param to 出力位置 最大2倍のサイズが必要
 *
 * @return 移動後の begin, to
 */
template <class UTF16Iterator, class UTF32Inserter,
    class = typename std::iterator_traits<UTF16Iterator>::value_type,
    class = typename std::iterator_traits<UTF32Inserter>::value_type
>
constexpr auto EncodingUTF16ToUTF32(UTF16Iterator begin, UTF16Iterator end, UTF32Inserter to)
    noexcept(noexcept(*to++ = char32_t(*begin++)))
    -> std::pair<UTF16Iterator, UTF32Inserter>
{
    auto it = begin;
    while (it != end) {

        auto to32 = EncodingUTF16ToUTF32(it, end);

        // シーケンス中途で end なら終える
        auto c = std::get<1>(to32);
        if (c == EncodingErrorNotEnough) break;
        it = std::get<0>(to32);
        auto safe = ReplacementIfEncodingError(c);

        *to++ = safe;
    }
    return {it, to};
}

/**
 * @brief UTF16 を UTF8 に変換する
 * 
 * @param begin 読み込み開始位置
 * @param end 読み込み終了位置
 * @param to 出力位置
 * 
 * @return 移動後の begin, to
 */
template <class UTF16Iterator, class UTF8Inserter,
    class = typename std::iterator_traits<UTF16Iterator>::value_type,
    class = typename std::iterator_traits<UTF8Inserter>::value_type
>
constexpr auto EncodingUTF16ToUTF8(UTF16Iterator begin, UTF16Iterator end, UTF8Inserter to)
    noexcept(noexcept(*to++ = char8_t(*begin++)))
    -> std::pair<UTF16Iterator, UTF8Inserter>
{
    auto it = begin;
    while (it != end) {

        // UTF32 化
        auto to32 = EncodingUTF16ToUTF32(it, end);

        // シーケンス中途で end なら終える
        auto c = std::get<1>(to32);
        if (c == EncodingErrorNotEnough) break;        
        it = std::get<0>(to32);
        auto safe = ReplacementIfEncodingError(c);

        // UTF8 化
        auto to8 = Unicode::ConvertUTF32ToUTF8(&safe, to);
        to = std::get<1>(to8);
    }
    return {it, to};
}

/**
 * @brief UTF8 を UTF16 に変換する
 *
 * @param begin 読み込み開始位置
 * @param end 読み込み終了位置
 * @param to 出力位置 最大2倍のサイズが必要
 *
 * @return 移動後の begin, to
 */
template <class UTF8Iterator, class UTF16Inserter,
    class = typename std::iterator_traits<UTF8Iterator>::value_type,
    class = typename std::iterator_traits<UTF16Inserter>::value_type
>
constexpr auto EncodingUTF8ToUTF16(UTF8Iterator begin, UTF8Iterator end, UTF16Inserter to)
    noexcept(noexcept(*to++ = char16_t(*begin++)))
    -> std::pair<UTF8Iterator, UTF16Inserter>
{
    auto it = begin;
    while (it != end) {

        // UTF32 化
        auto to32 = EncodingUTF8ToUTF32(it, end);

        // シーケンス中途で end なら終える
        auto c = std::get<1>(to32);
        if (c == EncodingErrorNotEnough) break;
        it = std::get<0>(to32);
        auto safe = ReplacementIfEncodingError(c);

        // UTF16 化
        auto to16 = Unicode::ConvertUTF32ToUTF16(&safe, to);
        to = std::get<1>(to16);
    }
    return {it, to};
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
