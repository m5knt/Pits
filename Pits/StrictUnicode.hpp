/**
 * @brief "", L"", u"", U"" がユニコードで有るか確認する
 * @author Yukio KANEDA
 * @file 
 */

#ifndef PITS_STRICTUNICODE_HPP_
#define PITS_STRICTUNICODE_HPP_

/*
 *
 */

#include "Pits/String.hpp"

namespace Pits {
namespace StrictUnicode {

/*
 *
 */

/// "" がユニコードで有るか確認する
static_assert(String::MultiByteEncoding == Pits::String::UTF8);

/// L"" がユニコードで有るか確認する
static_assert(String::WideCharEncoding == Pits::String::UTF16 || String::WideCharEncoding == Pits::String::UTF32);

/// u"" がユニコードで有るか確認する
static_assert(String::Char16Encoding == Pits::String::UTF16);

/// U"" がユニコードで有るか確認する
static_assert(String::Char32Encoding == Pits::String::UTF32);

/*
 *
 */

} // namespace StrictUnicode
} // namespace Pits

/*
 *
 */

#endif

/*
 *
 */
