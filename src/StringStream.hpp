/**
 * @brief std::stringstream をやや使いやすくするシュガー的な物
 * @author Yukio KANEDA
 * @file
 */

#ifndef PITS_STRINGSTREAM_HPP_
#define PITS_STRINGSTREAM_HPP_

#include <sstream>
#include <string_view>

/*
 *
 */

namespace Pits {

/**
 * @brief ストリングストリーム関係の内部実装用名前空間
 */
namespace StringStreamImplement {

/**
 * @brief std::basic_stringbuf のプロテクトへアクセスするだけの継承クラス
 */
template <class Char, class Traits, class Allocator>
class ToStringViewImpl : public std::basic_stringbuf<Char, Traits, Allocator>
{
    using stringbuf = std::basic_stringbuf<Char, Traits, Allocator>;
    using string_view = std::basic_string_view<Char>;

    // ユーティリティクラスでしかないので作成不可にする
    ToStringViewImpl() = delete;

public:

    /// バッファを参照する string_view を返す
    static auto ToStringView(const stringbuf* sbuf) noexcept -> string_view
	{
        if (auto p = static_cast<const ToStringViewImpl*>(sbuf)) {
            if (p->pptr()) {
                return string_view(p->pbase(), p->pptr() - p->pbase());
            }
            else if (p->gptr()) {
                return string_view(p->eback(), p->egptr() - p->eback());
            }
        }
        return string_view();
    }

    /// バッファを参照する string_view を返す
    static auto ToStringView(stringbuf* sbuf) noexcept -> string_view
	{
        if (auto p = static_cast<ToStringViewImpl*>(sbuf)) {
            if (p->pptr()) {
                return string_view(p->pbase(), p->pptr() - p->pbase());
            }
            else if (p->gptr()) {
                return string_view(p->eback(), p->egptr() - p->eback());
            }
        }
        return string_view();
    }
};

/*
 *
 */

} // namespace StringStreamImplement

/**
 * @brief バッファを参照する string_view を返す
 * @param ss ストリングストリーム
 * @return ストリングビュー
 */
template <class Char, class Traits, class Allocator>
inline
auto ToStringView(const std::basic_stringstream<Char, Traits, Allocator>& ss) noexcept
        -> std::basic_string_view<Char>
{
    using impl = StringStreamImplement::ToStringViewImpl<Char, Traits, Allocator>;
    return impl::ToStringView(ss.rdbuf());
}

/**
 * @brief バッファを参照する string_view を返す
 * @param ss ストリングストリーム
 * @return ストリングビュー
 */
template <class Char, class Traits, class Allocator>
inline
auto ToStringView(std::basic_stringstream<Char, Traits, Allocator>& ss) noexcept
        -> std::basic_string_view<Char>
{
    using impl = StringStreamImplement::ToStringViewImpl<Char, Traits, Allocator>;
    return impl::ToStringView(ss.rdbuf());
}

/**
 * @brief バッファを参照する string_view を返す
 * @param sb ストリングバッファーポインタ
 * @return ストリングビュー
 */
template <class Char, class Traits, class Allocator>
inline
auto ToStringView(const std::basic_stringbuf<Char, Traits, Allocator>* sb) noexcept
        -> std::basic_string_view<Char>
{
    using impl = StringStreamImplement::ToStringViewImpl<Char, Traits, Allocator>;
	return impl::ToStringView(sb);
}

/**
 * @brief バッファを参照する string_view を返す
 * @param sb ストリングバッファーポインタ
 * @return ストリングビュー
 */
template <class Char, class Traits, class Allocator> 
inline
auto ToStringView(std::basic_stringbuf<Char, Traits, Allocator>* sb) noexcept
        -> std::basic_string_view<Char>
{
    using impl = StringStreamImplement::ToStringViewImpl<Char, Traits, Allocator>;
	return impl::ToStringView(sb);
}

/*
 *
 */

} // namespace Pits

/*
 *
 */

#endif

/*
 *
 */
