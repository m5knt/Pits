/**
 * @brief 逆行しないクロックによるタイマー
 * @file
 * @author Yukio KANEDA
 */

#ifndef PITS_TIMER_HPP_
#define PITS_TIMER_HPP_

#include <chrono>       // chrono, ratio
#include <cstdint>      // intXX_t, uintXX_t
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
template <class SecondsType, class Duration>
inline
auto ToSeconds(const Duration& d) -> SecondsType
{
	using namespace std::chrono;
    return duration_cast<duration<SecondsType, std::ratio<1>>>(d).count();
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

    /// 時刻の型
    using TimePointType = ClockType::time_point;

    /// 秒表現に使う型 (doubleは仮数部52ビット)
    using SecondsType = double;

    /// 整数秒表現に使う型
    using IntegerSecondsType = std::int64_t;

    /// 分解能は少なくとも 10[usec] を要求
    static_assert(ClockType::period::num == 1 && ClockType::period::den >= 0'100'000);

    Timer(const Timer&) = default;
    Timer& operator=(const Timer&) = default;
    Timer(Timer&&) = default;
    Timer& operator=(Timer&&) = default;

    ~Timer() noexcept;

    /// 現在の時刻からタイマーを開始する
    Timer() noexcept;

    /// 指定の時刻からタイマーを開始する
    Timer(TimePointType tp) noexcept;

    /// 経過時間[sec]を返す
    auto GetElapsed() const noexcept -> SecondsType;

    /// 経過時間[sec]を返すとともにタイマーをリセットする
    auto GetElapsedAndReset() noexcept -> SecondsType;

    /// 整数経過時間[sec]を返すとともにタイマーをリセットする
    auto GetIntegerElapsedAndReset() noexcept -> IntegerSecondsType;

    /// タイマーをリセットする
    void Reset() noexcept;

    /// 現在時刻を返す
    static auto Now() noexcept -> TimePointType
    {
        return ClockType::now();
    }

private:

    /// 計測開始時刻
    TimePointType begin_;
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
