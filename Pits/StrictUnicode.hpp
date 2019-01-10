/**
 * @brief ユニコード関係
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

constexpr auto MultiByteEncoding = Pits::String::DetectKanjiEncoding("漢字")
constexpr auto WideCharEncoding = Pits::String::DetectKanjiEncoding(L"漢字")

static_assert(MultiByteEncoding == Pits::String::UTF8);
static_assert(WideCharEncoding == Pits::String::UTF16 || WideCharEncoding == Pits::String::UTF32);

/*
 *
 */

#if !defined(__STDC_UTF_16__) || !defined(__STDC_UTF_32__)
static_assert(false);
#endif

} // namespace StrictUnicode
} // namespace Pits

/*
 *
 */

#endif

/*
 *
 */
