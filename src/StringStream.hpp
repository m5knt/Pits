/**
 * @brief std::stringstream をやや使いやすくするシュガー的な物
 * @author Yukio KANEDA
 * @file
 */

#ifndef PITS_STRINGSTREAM_HPP_
#define PITS_STRINGSTREAM_HPP_

#include <memory>       // allocator
#include <string>       // char_traits
#include <string_view>
#include <sstream>

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
    /// 基底となる stringbuf の型
    using stringbuf = std::basic_stringbuf<Char, Traits, Allocator>;

    /// バッファを参照する型
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

/*
 *
 */

/**
 * @brief バッファを参照する string_view を返す
 * @param sb ストリングバッファーポインタ
 * @return ストリングビュー
 */
template <class Char, class Traits, class Allocator>
using ToStringView = StringStreamImplement::ToStringViewImpl<Char, Traits, Allocator>;

/**
 * @brief str() で string_view を返す stringstream の派生クラス
 */
template <class Char, class Traits = std::char_traits<Char>, class Allocator =  std::allocator<Char>>
class BasicStringStream : public std::basic_stringstream<Char, Traits, Allocator> {

public:

    /// 基底となる basic_stringstream の型
    using stringstream = std::basic_stringstream<Char, Traits, Allocator>;

    /// バッファデバイスとなる basic_stringbuf の型
    using stringbuf = std::basic_stringbuf<Char, Traits, Allocator>;

    /// バッファを参照する型
    using string_view = std::basic_string_view<Char>;

    using stringstream::stringstream;

    /**
     * @brief バッファの内容を string_view 型のオブジェクトで返す
     * @return string_view 
     */
    auto str() const -> string_view
    {
        using impl = StringStreamImplement::ToStringViewImpl<Char, Traits, Allocator>;
        return impl::ToStringView(rdbuf());
    };

};

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
