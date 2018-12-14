/**
 * @file 
 * @author yukio kaneda
 * @brief 最小限の文字列エンコーディングユーティリティー関係
 */

#ifndef PITS_ENCODING_HPP_
#define PITS_ENCODING_HPP_

#include <cstdint>  // uint8_t
#include <string>   // string, wstring, u16string

/*
 *
 */

namespace Pits {

/// 最小限の文字列エンコーディングユーティリティー関係
inline namespace Encoding {

/*
 *
 */

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
constexpr auto GetEncodingType(const char* kan) -> EncodingType
{
    using c = std::uint8_t;
    auto k = kan;
    auto j = SJISKan;
    auto u = UTF8Kan;

    if (c(k[0]) == j[0] && c(k[1]) == j[1]) {
        return EncodingType::SJIS;
    }
    else if (c(k[0]) == u[0] && c(k[1]) == u[1] && c(k[2]) == u[2]) {
        return EncodingType::UTF8;
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
constexpr auto IsUTF8Encoding(const char* kan) -> bool
{
    return GetEncodingType(kan) == EncodingType::UTF8;
}

/**
 * @brief マルチバイト文字列がSJIS(CP932)エンコードであるか返す
 * @param kan 文字列 "漢" を渡す
 * @return 真偽
 * 
 * static_assert(IsSJISEncoding("漢"));
 */
constexpr auto IsSJISEncoding(const char* kan) -> bool
{
    return GetEncodingType(kan) == EncodingType::SJIS;
}

/// UTF16 L"漢"
constexpr char16_t UTF16Kan[] = { 0x6f22, 0 };

/// UTF32 L"漢"
constexpr char32_t UTF32Kan[] = { 0x00006f22, 0 };

/**
 * @brief ワイド文字列のエンコード種別を返す
 * @param kan 文字列 L"漢" を渡す
 * @return EncodingType 
 */
constexpr auto GetEncodingType(const wchar_t* kan) -> EncodingType
{
    using c = unsigned int;
    auto k = kan;
    auto u16 = UTF16Kan;
    auto u32 = UTF32Kan;

    if ((sizeof(k) == sizeof(u16)) && (c(k[0]) == u16[0])) {
        return EncodingType::UTF16;
    }
    else if ((sizeof(k) == sizeof(u32)) && (c(k[0]) == u32[0])) {
        return EncodingType::UTF32;
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
 * static_assert(IsUTF16Encoding(L"漢"));
 */
constexpr auto IsUTF16Encoding(const wchar_t* kan) -> bool
{
    return GetEncodingType(kan) == EncodingType::UTF16;
}

/**
 * @brief ワイド文字列がUTF32エンコードであるか返す
 * @param kan 文字列 L"漢" を渡す
 * @return 真偽
 * 
 * static_assert(IsUTF32Encoding(L"漢"));
 */
constexpr auto IsUTF32Encoding(const wchar_t* kan) -> bool
{
    return GetEncodingType(kan) == EncodingType::UTF32;
}

/**
 * @brief サロゲートコードで有るか返す
 * @param c 文字
 * @return bool 真偽
 */
constexpr auto IsSurrogate(int c) -> bool
{
    return 0xd800 <= c && c <= 0xdfff;
}

/**
 * @brief ハイサロゲートコードで有るか返す
 * @param c 文字
 * @return bool 真偽
 */
constexpr auto IsHighSurrogate(int c) -> bool
{
    return 0xd800 <= c && c <= 0xdbff;
}

/**
 * @brief ローサロゲートコードで有るか返す
 * @param c 文字
 * @return bool 真偽
 */
constexpr auto IsLowSurrogate(int c) -> bool
{
    return 0xdc00 <= c && c <= 0xdfff;
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
