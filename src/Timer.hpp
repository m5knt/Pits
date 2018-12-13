/**
 * @brief 逆行しないクロックによるタイマー
 * @file
 * @author Yukio KANEDA
 */

#ifndef PITS_TIMER_HPP_
#define PITS_TIMER_HPP_

#include <chrono>       // chrono, ratio
#include <cstdint>      // int64_t
#include <type_traits>  // conditional_t

/*
 *
 */

namespace Pits {

/**
 * @brief 時間(std::chrono::duration)を任意の型の秒にする
 * @param d 時間 duration 型である事
 * @return 秒
 */
template <class Rep, class Period, class SecondType = double>
inline
auto ToSecond(std::chrono::duration<Rep, Period> d) -> SecondType
{
    return std::chrono::duration_cast<SecondType, std::ratio<1>>(d).count();
    //return SecondType(d.count()) * Period::num / Period::den;
}

/**
 * @brief 逆行しないクロックによるタイマー
 * クロックは逆行しない std::chrono::high_resolution_clock もしくは
 * std::chrono::steady_clock 型が選ばれる
 * クロックのカウント型が64ビットナノ秒実装であれば
 * 最大9'223'372'036.854'775'807秒扱える
 */
class Timer
{
public:

    /// 使うクロックの型 逆行はバグの元なのでさせない
    using ClockType = std::conditional_t<
        std::chrono::high_resolution_clock::is_steady,
        std::chrono::high_resolution_clock,
        std::chrono::steady_clock
        >;

    /// 秒表現に使う型 (doubleは仮数部52ビット)
    using SecondType = double;

    /// 整数秒表現に使う型
    using IntegerSecondType = std::int64_t;

    /// 分解能は少なくとも 10[usec] を要求
    static_assert(ClockType::period::den >= 0'100'000);

    Timer(const Timer&) = default;
    Timer& operator=(const Timer&) = default;
    Timer(Timer&&) = default;
    Timer& operator=(Timer&&) = default;

    ~Timer() noexcept;

    /// 現在の時刻からタイマーを開始する
    Timer() noexcept;

    /// 指定の時刻からタイマーを開始する
    Timer(ClockType::time_point tp) noexcept;

    /// 経過時間[sec]を返す
    auto GetElapsed() const noexcept -> SecondType;

    /// 経過時間[sec]を返すとともにタイマーをリセットする
    auto GetElapsedAndReset() noexcept -> SecondType;

    /// 整数経過時間[sec]を返すとともにタイマーをリセットする
    auto GetIntegerElapsedAndReset() noexcept -> IntegerSecondType;

    /// タイマーをリセットする
    void Reset() noexcept;

private:

    /// 計測開始時刻
    ClockType::time_point begin_;
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
