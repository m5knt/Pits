/**
 * @brief 最低最悪土壺状態から壺を心得る事を目標としながら偏狭な陶芸家の様に壺を割るライブラリ "The Pits"
 * @author yukio kaneda
 * @file 
 */

#ifndef PITS_PITS_HPP_
#define PITS_PITS_HPP_

/*
 *
 */

#include "Encoding.hpp"
#include "Timer.hpp"

#include <thread>       // this_thread thread::id
#include <chrono>       // chrono_literals
#include <string>       // string_literals
#include <string_view>  // string_view_literals

/**
 * @brief 最低最悪土壺状態から壺を心得る事を目標としながら偏狭な陶芸家の様に壺を割るライブラリ "The Pits"
 */
namespace Pits {

/**
 * @brief 各種動作状況の纏め
 */
class Pits {

public:

    Pits(const Pits&) = delete;
    Pits& operator=(const Pits&) = delete;
    Pits(Pits&&) = delete;
    Pits& operator=(Pits&&) = delete;

    ~Pits();
    Pits();

public:

    /// ライブラリビルド時のマルチバイト文字列エンコード種別
    auto GetMultiByteEncoding() const noexcept -> EncodingType;

    /// ライブラリビルド時のワイド文字列エンコード種別
    auto GetWideCharEncoding() const noexcept -> EncodingType;

    /*
     *
     */

    /// 定期集計処理 (メインスレッド)
    auto Update() -> void;

private:

    /// メインスレッドのId
    const std::thread::id main_thread_id_;

public:

    /// 現在のスレッドがメインスレッドで有るか返す (スレッドセーフ)
    auto IsMainThread() const noexcept -> bool
    {
        return std::this_thread::get_id() == main_thread_id_;
    }

    /*
     *
     */

private:

    /// 再訪間隔測用タイマー
	Timer revisit_timer_ {};

    /// 最大訪間時間[sec]
    double revisit_max_ {};

    /// 経過時間計測用タイマー
    Timer elapsed_timer_ {};

    /// 経過時間[sec]
    std::int64_t elapsed_ {};

public:

    /// 最大再訪間隔 (メインスレッド)
    auto GetRevisitMax() const noexcept -> double;

    /// 経過時間[秒]を返す (メインスレッド)
    auto GetIntegerElapsed() noexcept -> std::int64_t;

    // auto GetBeginTime() const noexcept -> std::time_t;

    // /// 起動時のシステム時刻 (逆行)
    // std::chrono::system_clock::time_point BeginSystemClock;
};

/*
 *
 */

/// 各種動作状況の纏め
extern class Pits Pits;

/// 現在のスレッドがメインスレッドで有るか返す (スレッドセーフ)
inline
auto IsMainThread() noexcept -> bool
{
    return Pits.IsMainThread();
}

/*
 *
 */

} // namespace Pits

/*
 *
 */

namespace Pits {
namespace Standard {

/*
 *
 */

using namespace std::string_literals;
using namespace std::string_view_literals;
using namespace std::chrono_literals;
// using namespace std::complex_literals;

/*
 *
 */

} // namespace Literals
} // namespace Pits

/*
 *
 */

#endif

/*
 *
 */
