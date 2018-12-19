/**
 * @brief 文字列エンコーディングユーティリティー
 * @author Yukio KANEDA
 * @file 
 */

#ifndef PITS_ENCODING_HPP_
#define PITS_ENCODING_HPP_

#include <cstdint>  // uint8_t
#include <string>   // string, wstring, u16string

/*
 *
 */

namespace Pits {

/// 文字列エンコーディングユーティリティー
inline namespace Encoding {

/*
 * char8_t   8ビットの符号無し整数 __STDC_UTF_8__  マクロ定義時に UTF8保証(予定 C2x:n2231)
 * char16_t 16ビットの符号付き整数 __STDC_UTF_16__ マクロ定義時に UTF16保証
 * char32_t 32ビットの符号付き整数 __STDC_UTF_32__ マクロ定義時に UTF32保証
 * 
 * ユニコード文字は 10ffff まで
 * Name |Bytes|            Bits|   Ranges| Remarks
 * -----+-----+----------------+---------+------------------------
 * UTF32|    4|              32|ffff'ffff|
 * UTF16|    2|              16|    'ffff|内 d800-dfff サロゲートペア
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

/**
 * @brief ライブラリビルド時における "" と L"" のエンコード種別
 * プログラミング環境として利用が見込まれる種別のみ扱います
 */
enum class EncodingType : std::uint8_t {
    UTF8,
    UTF16,
    UTF32,
    SJIS, ///< CP932, MS932, Windows31J
    OTHER,
};

/// SJIS "漢"
constexpr std::uint8_t SJISKan[] = { 0x8a, 0xbf, 0x00 };

/// UTF8 "漢"
constexpr std::uint8_t UTF8Kan[] = { 0xe6, 0xbc, 0xa2, 0x00 };

/**
 * @brief マルチバイト文字列のエンコード種別を返す
 * @param kan 文字列 "漢" を渡す
 * @return EncodingType 
 */
constexpr auto GetEncodingType(const char* kan) noexcept -> EncodingType
{
    using c = std::uint8_t;
    auto k = kan;
    auto j = SJISKan;
    auto u = UTF8Kan;

    if (k) {
        if ((c(k[0]) == j[0]) && (c(k[1]) == j[1])) {
            return EncodingType::SJIS;
        }
        else if ((c(k[0]) == u[0]) && (c(k[1]) == u[1]) && (c(k[2]) == u[2])) {
            return EncodingType::UTF8;
        }
    }
    else {
        return EncodingType::OTHER;
    }
}

/**
 * @brief マルチバイト文字列がUTF8エンコードであるか返す
 * @param kan 文字列 "漢" を渡す
 * @return 真偽
 * 
 * static_assert(IsUTF8Encoding("漢"));
 */
constexpr auto IsUTF8Encoding(const char* kan) noexcept -> bool
{
    return kan && (GetEncodingType(kan) == EncodingType::UTF8);
}

/**
 * @brief マルチバイト文字列がSJIS(CP932)エンコードであるか返す
 * @param kan 文字列 "漢" を渡す
 * @return 真偽
 * 
 * static_assert(IsSJISEncoding("漢"));
 */
constexpr auto IsSJISEncoding(const char* kan) noexcept -> bool
{
    return kan && (GetEncodingType(kan) == EncodingType::SJIS);
}

/// UTF16 L"漢"
constexpr char16_t UTF16Kan[] = { 0x6f22, 0 };

/// UTF32 L"漢"
constexpr char32_t UTF32Kan[] = { 0x00006f22, 0 };

/**
 * @brief ワイド文字列のエンコード種別を返す
 * @param kan 文字列 L"漢" を渡す
 * @return EncodingType 
 * @note Windows は UTF16, Linux/OSX は UTF-32 が一般的である
 */
constexpr auto GetEncodingType(const wchar_t* kan) noexcept -> EncodingType
{
    using c = unsigned int;
    auto k = kan;
    auto u16 = UTF16Kan;
    auto u32 = UTF32Kan;

    if (k) {
        if ((sizeof(k) == sizeof(u16[0])) && (c(k[0]) == u16[0])) {
            return EncodingType::UTF16;
        }
        else if ((sizeof(k) == sizeof(u32[0])) && (c(k[0]) == u32[0])) {
            return EncodingType::UTF32;
        }
    }
    else {
        return EncodingType::OTHER;
    }
}

/**
 * @brief ワイド文字列がUTF16エンコードであるか返す
 * @param kan 文字列 L"漢" を渡す
 * @return 真偽
 * 
 * @note static_assert(IsUTF16Encoding(L"漢"));
 */
constexpr auto IsUTF16Encoding(const wchar_t* kan) noexcept -> bool
{
    return kan && (GetEncodingType(kan) == EncodingType::UTF16);
}

/**
 * @brief char16_t文字列がUTF16エンコードであるか返す
 * @param kan 文字列 u"漢" を渡す
 * @return 真偽
 * 
 * @note static_assert(IsUTF16Encoding(u"漢"));
 */
constexpr auto IsUTF16Encoding(const char16_t* kan) noexcept -> bool
{
    return kan && (kan[0] == UTF16Kan[0]);
}

/**
 * @brief ワイド文字列がUTF32エンコードであるか返す
 * 
 * @param kan 文字列 L"漢" を渡す
 * @return 真偽
 * 
 * @note static_assert(IsUTF32Encoding(L"漢"));
 */
constexpr auto IsUTF32Encoding(const wchar_t* kan) noexcept -> bool
{
    return kan && (GetEncodingType(kan) == EncodingType::UTF32);
}

/**
 * @brief char32_t文字列がUTF32エンコードであるか返す
 * @param kan 文字列 U"漢" を渡す
 * @return 真偽
 * 
 * @note static_assert(IsUTF32Encoding(U"漢"));
 */
constexpr auto IsUTF32Encoding(const char32_t* kan) noexcept -> bool
{
    return kan && (kan[0] == UTF32Kan[0]);
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
 * @brief UTF8イテレータが示す位置のUTF8文字をUTF32文字へ一文字デコードしイテレータを進める
 * @param it デコード位置
 * @param end コンテナ終端位置
 * @return UTF32文字(失敗時はUTF32ErrorCharactor)
 */
template <class UTF8Iterator>
auto UTF8ToUTF32(UTF8Iterator& it, UTF8Iterator end) noexcept -> char32_t
{
    if (it == end) {
        return UnicodeErrorCharacter;
    }

    auto c = *it++ & 0xff;
    auto remain = int{};

    // ascii文字であるか
    if (c <= 0x7f) {
        return c;
    }
    else if ((c & 0b0'1110'0000) == 0b0'1100'0000) {
        c &= 0b0'0001'1111;
        remain = 1;
    }
    else if ((c & 0b0'1111'0000) == 0b0'1110'0000) {
        c &= 0b0'0000'1111;
        remain = 2;
    }
    else if ((c & 0b0'1111'1000) == 0b0'1111'0000) {
        c &= 0b0'0000'0111;
        remain = 3;
    } else {
        // 不正データ
        return UnicodeErrorCharacter;
    }

    // マルチバイト文字
    for (; remain && (it != end) && ((*it & 0b0'1100'0000) == 0b0'1000'0000); --remain, ++it) {
        c = (c << 6) | (*it & 0b0'0011'1111);
    }
    return !remain ? c : UnicodeErrorCharacter;
}

/**
 * @brief UTF16イテレータが示す位置のUTF16文字をUTF32文字へ一文字デコードしイテレータを進める
 * @param it デコード位置
 * @param end コンテナ終端位置
 * @return UTF32文字(失敗時はUTF32ErrorCharactor)
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
            return c;
        }
        return UnicodeErrorCharacter;
    }
    return UnicodeErrorCharacter;
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
