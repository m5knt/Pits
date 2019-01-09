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
constexpr char32_t EncodingErrorIllegalSequence = -1;

/// エンコーディングに必要なデータが足りない (UTF32と被らない値)
constexpr char32_t EncodingErrorNotEnough = -2;

/**
 * @brief ライブラリビルド時における "" と L"" のエンコード種別
 * ビルド環境として利用が見込まれる種別のみ扱います
 */
enum class EncodingType : std::uint8_t
{
    // マルチバイト系
    UTF8,       ///< UTF8  1 ～ 4 Bytes
    SJIS,       ///< SJIS  1 ～ 2 Bytes (Aka CP932, MS932, Windows31J)

    // ワイド系
    UTF16,      ///< UTF16 (Native)
    UTF32,      ///< UTF32 (Native)

    // その他
    OTHER,
};

/**
 * @brief 文字列エンコーディング関係の内部実装
 */
namespace EncodingImplement {

/*
 *
 */

template <class Char>
struct KanjiEncoding;

template <>
struct KanjiEncoding<char> {

    static constexpr auto GetKanjiEncodingType(const char* target) noexcept
        -> EncodingType
    {
        // 以下4つはプログラムソースで表現できないはずなので評価しない
        // UTF16BE {0x6f, 0x22, 0x5b, 0x57};
        // UTF16LE {0x22, 0x6f, 0x57, 0x5b};
        // UTF32BE {0x00, 0x00, 0x6f, 0x22, 0x00, 0x00, 0x5b, 0x57};
        // UTF32LE {0x22, 0x6f, 0x00, 0x00, 0x57, 0x5b, 0x00, 0x00};

        using cast = std::uint8_t;
        {
            // UTF8 1 ～ 4バイト
            constexpr std::uint8_t source[] = {0xe6, 0xbc, 0xa2, 0xe5, 0xad, 0x97};
            auto s = source;
            auto t = target;
            if (cast(*t) == *s && cast(*++t) == *++s && cast(*++t) == *++s && cast(*++t) == *++s) {
                return EncodingType::UTF8;
            }
        }
        {
            // SJIS 1 ～ 2バイト
            constexpr std::uint8_t source[] = {0x8a, 0xbf, 0x8e, 0x9a};
            auto s = source;
            auto t = target;
            if (cast(*t) == *s && cast(*++t) == *++s && cast(*++t) == *++s && cast(*++t) == *++s) {
                return EncodingType::SJIS;
            }
        }
        return EncodingType::OTHER;
    }
};

template <>
struct KanjiEncoding<wchar_t> {

    static constexpr auto GetKanjiEncodingType(const wchar_t* target) noexcept -> EncodingType
    {
        if (sizeof(*target) == sizeof(char16_t)) {

            using cast = char16_t;
            char16_t source[] = {0x6f22, 0x5b57};
            auto s = source;
            auto t = target;
            if (cast(*t) == *s && cast(*++t) == *++s) {
                return EncodingType::UTF16;
            }
        }
        else if (sizeof(*target) == sizeof(char32_t)) {

            using cast = char32_t;
            char32_t source[] = {0x6f22, 0x5b57};
            auto s = source;
            auto t = target;
            if (cast(*t) == *s && cast(*++t) == *++s) {
                return EncodingType::UTF32;
            }
        }
        return EncodingType::OTHER;
    }
};

template <>
struct KanjiEncoding<char16_t> {
    static constexpr auto GetKanjiEncodingType(const char16_t* kanji) -> EncodingType
    {
#ifdef __STDC_UTF_16__
        char16_t source[] = {0x6f22, 0x5b57};
        auto s = source;
        auto t = kanji;
        return (*s == *t && *++s == *++t) ? EncodingType::UTF16 : EncodingType::OTHER;
#else
        return EncodingType::OTHER;
#endif
    }
};

template <>
struct KanjiEncoding<char32_t> {
    static constexpr auto GetKanjiEncodingType(const char32_t* kanji) -> EncodingType
    {
#ifdef __STDC_UTF_32__
        char32_t source[] = {0x6f22, 0x5b57};
        auto s = source;
        auto t = kanji;
        return (*s == *t && *++s == *++t) ? EncodingType::UTF32 : EncodingType::OTHER;
#else
        return EncodingType::OTHER;
#endif
    }
};

} // namespace EncodingImplement

/**
 * @brief リテラル "漢字" のエンコード種別を返す
 * 
 * C/C++言語仕様上 char[] wchar_t[] char16_t[] char32_t[] は
 * 特定のエンコードではない為、決め打ち実装時に厳密に判定出来るよう関数を用意している
 * u8"" の char8_t[] は UTF8 と決まっているので用意しない
 * 
 * @param kanji リテラル "漢字" L"漢字" u"漢字" U"漢字" を渡す
 * @return EncodingType
 * 
 * SJIS でコンパイルされないと問題の出るソース等で次の様にする
 * static_assert(Pits::GetKanjiEncodingType("漢字") == Pits::EncodingType::SJIS);
 */
template <class Char>
constexpr auto GetKanjiEncodingType(const Char* kanji) noexcept -> EncodingType
{
    return EncodingImplement::KanjiEncoding<Char>::GetKanjiEncodingType(kanji);
}

/**
 * @brief エンコーディングエラーなら文字を置き換える
 * @param from 文字
 * @return 文字
 */
constexpr auto ReplacementIfEncodingError(char32_t from) -> char32_t
{
    return from < Unicode::CharacterMax ? from : Unicode::ReplacementCharacter;
}

/**
 * @brief UTF32 コードを UTF8 へ変換する 事前に安全な文字で有るか確認する事
 * 
 * @param from UTF32 コード
 * @return [0] 常にデータあり / [1～] != 0 ならデータ有り
 */
constexpr auto EncodingUTF32ToUTF8Unsafe(char32_t from) -> std::array<char8_t, 4>
{
    auto c = from;

    if (c <= 0x7f) {
        auto c0 = char8_t(c);
        return {c0};
    }
    else if (c <= 0x7ff) {
        auto c1 = char8_t((c & 0b0'0011'1111) | 0b0'1000'0000); c >>= 6;
        auto c0 = char8_t((c & 0b0'0011'1111) | 0b0'1100'0000);
        return {c0, c1};
    }
    else if (c <= 0xffff) {
        auto c2 = char8_t((c & 0b0'0011'1111) | 0b0'1000'0000); c >>= 6;
        auto c1 = char8_t((c & 0b0'0011'1111) | 0b0'1000'0000); c >>= 6;
        auto c0 = char8_t((c & 0b0'0001'1111) | 0b0'1110'0000);
        return {c0, c1, c2};
    }
    else {
        auto c3 = char8_t((c & 0b0'0011'1111) | 0b0'1000'0000); c >>= 6;
        auto c2 = char8_t((c & 0b0'0011'1111) | 0b0'1000'0000); c >>= 6;
        auto c1 = char8_t((c & 0b0'0011'1111) | 0b0'1000'0000); c >>= 6;
        auto c0 = char8_t((c & 0b0'0000'1111) | 0b0'1111'0000);
        return {c0, c1, c2, c3};
    }
}

/**
 * @brief UTF32 コードを UTF8 へ変換する 事前に安全な文字で有るか確認する事
 * @param from UTF32 コード
 * @param out 出力イテレーター
 * @return 移動した出力イテレーター
 */
template <class UTF8Iterator,
    class Require = typename std::iterator_traits<UTF8Iterator>::value_type
>
constexpr auto EncodingUTF32ToUTF8Unsafe(char32_t from, UTF8Iterator out) -> UTF8Iterator
{
    auto c = from;

    if (c <= 0x7f) {
        auto c0 = char8_t(c);
        *out++ = c0;
    }
    else if (c <= 0x7ff) {
        auto c1 = char8_t((c & 0b0'0011'1111) | 0b0'1000'0000); c >>= 6;
        auto c0 = char8_t((c & 0b0'0011'1111) | 0b0'1100'0000);
        *out++ = c0;
        *out++ = c1;
    }
    else if (c <= 0xffff) {
        auto c2 = char8_t((c & 0b0'0011'1111) | 0b0'1000'0000); c >>= 6;
        auto c1 = char8_t((c & 0b0'0011'1111) | 0b0'1000'0000); c >>= 6;
        auto c0 = char8_t((c & 0b0'0001'1111) | 0b0'1110'0000);
        *out++ = c0;
        *out++ = c1;
        *out++ = c2;
    }
    else {
        auto c3 = char8_t((c & 0b0'0011'1111) | 0b0'1000'0000); c >>= 6;
        auto c2 = char8_t((c & 0b0'0011'1111) | 0b0'1000'0000); c >>= 6;
        auto c1 = char8_t((c & 0b0'0011'1111) | 0b0'1000'0000); c >>= 6;
        auto c0 = char8_t((c & 0b0'0000'1111) | 0b0'1111'0000);
        *out++ = c0;
        *out++ = c1;
        *out++ = c2;
        *out++ = c3;
    }
    return out;
}

/**
 * @brief UTF32 文字を UTF16 へ変換する 事前に安全な文字で有るか確認する事
 * @param from UTF32 コード
 * @return [0] 常にデータあり / [1] != 0 ならデータ有り
 */
constexpr auto EncodingUTF32ToUTF16Unsafe(char32_t from) noexcept -> std::array<char16_t, 2>
{
    auto c = from;

    if (c <= 0xffff) {
        return {char16_t(c)};
    }
    else {
        auto h = char16_t(((c - 0x10000) / 0x400) + 0xd800);
        auto l = char16_t(((c - 0x10000) & 0x3ff) + 0xdc00);
        return {h, l};
    }
}

/**
 * @brief UTF32 コードを UTF16 へ変換する 事前に安全な文字で有るか確認する事
 * @param from UTF32 コード
 * @param out 出力イテレーター
 * @return 移動した出力イテレーター
 */
template <class UTF16Iterator,
    class Require = typename std::iterator_traits<UTF16Iterator>::value_type
>
constexpr auto EncodingUTF32ToUTF16Unsafe(char32_t from, UTF16Iterator out) noexcept -> UTF16Iterator
{
    auto c = from;

    if (c <= 0xffff) {
        auto t = char16_t(c);
        *out++ = t;
    }
    else {
        auto h = char16_t(((c - 0x10000) / 0x400) + 0xd800);
        auto l = char16_t(((c - 0x10000) & 0x3ff) + 0xdc00);
        *out++ = h;
        *out++ = l;
    }
    return out;
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
    class Require = typename std::iterator_traits<UTF8Iterator>::value_type
>
constexpr auto EncodingUTF8ToUTF32(UTF8Iterator begin, UTF8Iterator end = UTF8Iterator()) noexcept
    -> std::pair<char32_t, UTF8Iterator>
{
    auto it = begin;

    // シングルコードの確認
    auto c = char32_t(*it++ & 0xff);
    if (c < 0b0'1000'0000) {        //  0 ～ 7f 0 ～ 7f 7
        return {c, it};
    }

    auto min = char32_t {};
    auto req = int {};

    /**/ if (c < 0b0'1100'0000) {   // 80 ～ bf
        // 読み込みコードポイント毎に置き換え
        return {EncodingErrorIllegalSequence, it};
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
        return {EncodingErrorIllegalSequence, ++begin};
    }

    // マルチバイト後続確認
    for (; req; --req) {

        // 中途で終了か
        if (it == end) {
            return {EncodingErrorNotEnough, ++begin};
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
            if (c < min || Unicode::IsUnsafeUTF32(c)) {
                return {EncodingErrorIllegalSequence, ++begin};
            }

            // 進めたコードポイント範囲を置き換え
            return {EncodingErrorIllegalSequence, it};
        }
    }

    // 不正コードなら読み込みコードポイント毎に置き換え
    if (c < min || Unicode::IsUnsafeUTF32(c)) {
        return {EncodingErrorIllegalSequence, ++begin};
    }

    // 成功
    return {c, it};
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
    class Require = typename std::iterator_traits<UTF16Iterator>::value_type
>
constexpr auto EncodingUTF16ToUTF32(UTF16Iterator begin, UTF16Iterator end = UTF16Iterator()) noexcept
    -> std::pair<char32_t, UTF16Iterator>
{
    auto it = begin;

    auto c = char32_t(*it++ & 0xffff);

    // ハイサロゲートか
    if (Unicode::IsHighSurrogate(c)) {

        if (it == end) {
            return {EncodingErrorNotEnough, it};
        }

        // ローサロゲートか
        auto l = char32_t(*it & 0xffff);
        if (Unicode::IsLowSurrogate(l)) {
            c = 0x10000 + ((c - 0xd800) * 0x400) + (l - 0xdc00);
            ++it;
        }
        else {
            // 進めたコードポイント範囲を置き換え
            return {EncodingErrorIllegalSequence, it};
        }
    }

    // 不正コードなら置き換え
    if (Unicode::IsUnsafeUTF32(c)) {
        return {EncodingErrorIllegalSequence, it};
    }

    // 成功
    return {c, it};
}

/**
 * @brief UTF8 を UTF32 に変換する
 * 
 * @param begin 読み込み開始位置
 * @param end 読み込み終了位置
 * @param out 出力位置
 * 
 * @return UTF8InIter 進んだ読み込み位置
 */
template <class UTF8InIter, class UTF32Inserter,
    class Require0 = typename std::iterator_traits<UTF8InIter>::value_type,
    class Require1 = typename std::iterator_traits<UTF32Inserter>::value_type
>
constexpr auto EncodingUTF8ToUTF32(UTF8InIter begin, UTF8InIter end, UTF32Inserter out) -> UTF8InIter
{
    auto it = begin;
    while (it != end) {

        auto to32 = EncodingUTF8ToUTF32(it, end);
        auto c = std::get<0>(to32);

        // シーケンス中途で end なら終える
        if (c == EncodingErrorNotEnough) break;

        // 書き込み
        c = ReplacementIfEncodingError(c);
        it = std::get<1>(to32);
        *out++ = c;
    }
    return it;
}

/**
 * @brief UTF8 を UTF16 に変換する
 * 
 * @param begin 読み込み開始位置
 * @param end 読み込み終了位置
 * @param out 出力位置
 * 
 * @return UTF8InIter 読み進んだ位置
 */
template <class UTF8InIter, class UTF16Inserter,
    class Require0 = typename std::iterator_traits<UTF8InIter>::value_type,
    class Require1 = typename std::iterator_traits<UTF16Inserter>::value_type
>
constexpr auto EncodingUTF8ToUTF16(UTF8InIter begin, UTF8InIter end, UTF16Inserter out) -> UTF8InIter
{
    auto it = begin;
    while (it != end) {

        // UTF32 を経由する
        auto to32 = EncodingUTF8ToUTF32(it, end);
        auto c = std::get<0>(to32);

        // シーケンス中途で end なら終える
        if (c == EncodingErrorNotEnough) break;

        c = ReplacementIfEncodingError(c);
        it = std::get<1>(to32);

        // 書き込み
        out = EncodingUTF32ToUTF16Unsafe(c, out);
    }
    return it;
}

/**
 * @brief UTF32 を UTF8 に変換する
 * 
 * @param begin 読み込み開始位置
 * @param end 読み込み終了位置
 * @param out 出力位置
 * 
 * @return UTF32InIter 読み進んだ位置
 */
template <class UTF32InIter, class UTF8Inserter,
    class Require0 = typename std::iterator_traits<UTF32InIter>::value_type,
    class Require1 = typename std::iterator_traits<UTF8Inserter>::value_type
>
constexpr auto EncodingUTF32ToUTF8(UTF32InIter begin, UTF32InIter end, UTF8Inserter out) -> UTF32InIter
{
    auto it = begin;
    while (it != end) {
        auto c = ReplacementIfEncodingError(*it++);
        out = EncodingUTF32ToUTF8Unsafe(c, out);
    }
    return it;
}

/**
 * @brief UTF16 を UTF8 に変換する
 * 
 * @param begin 読み込み開始位置
 * @param end 読み込み終了位置
 * @param out 出力位置
 * 
 * @return UTF16InIter 読み進んだ位置
 */
template <class UTF16InIter, class UTF8Inserter,
    class Require0 = typename std::iterator_traits<UTF16InIter>::value_type,
    class Require1 = typename std::iterator_traits<UTF8Inserter>::value_type
>
constexpr auto EncodingUTF16ToUTF8(UTF16InIter begin, UTF16InIter end, UTF8Inserter out) -> UTF16InIter
{
    auto it = begin;
    while (it != end) {

        // UTF32 を経由する
        auto to32 = EncodingUTF16ToUTF32(it, end);
        auto c = std::get<0>(to32);

        // シーケンス中途で end なら終える
        if (c == EncodingErrorNotEnough) break;

        c = ReplacementIfEncodingError(c);
        it = std::get<1>(to32);
        out = EncodingUTF32ToUTF8Unsafe(c, out);
    }
    return it;
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
