/**
 * @brief 逆行しないクロックによるタイマー
 * @file 
 * @author Yukio KANEDA
 */

#include "Timer.hpp"
#include <new>          // placement new

/*
 *
 */

namespace Pits {

/*
 *
 */

Timer::~Timer() noexcept
{}

// タイマーを開始する
Timer::Timer() noexcept
: begin_ {ClockType::now()}
{
}

// 指定時刻からタイマーを開始する
Timer::Timer(ClockType::time_point tp) noexcept
: begin_ {tp}
{
}

// 経過時間[sec]を返す
auto Timer::GetElapsed() const noexcept -> SecondType
{
    using namespace std::chrono;
    auto beg = begin_;
    auto now = ClockType::now();
    return ToSecond(now - beg);
}

// 経過時間[sec]を返すとともにタイマーをリセットする
auto Timer::GetElapsedAndReset() noexcept -> SecondType
{
    using namespace std::chrono;
    auto beg = begin_;
    auto now = ClockType::now();
    new(this) Timer(now);
    return ToSecond(now - beg);
}

// 整数経過時間[sec]を返すとともにタイマーをリセットする
auto Timer::GetIntegerElapsedAndReset() noexcept -> IntegerSecondType
{
    using namespace std::chrono;
    auto beg = begin_;
    auto now = ClockType::now();
    auto elp = now - beg;
    if (auto sec = elp / seconds(1)) {
        new(this) Timer(beg + seconds(sec));
        return sec;
    }
    return 0;
}

// タイマーをリセットする
void Timer::Reset() noexcept
{
    new(this) Timer();
}

/*
 *
 */

} // namespace Pits

/*
 *
 */
