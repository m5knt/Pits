/**
 * @brief 逆行しないクロックによるタイマー
 * @author Yukio KANEDA
 * @file 
 */

#include "Timer.hpp"
#include <new>          // new(place)

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
: begin_ {Now()}
{
}

// 指定時刻からタイマーを開始する
Timer::Timer(TimePointType tp) noexcept
: begin_ {tp}
{
}

// 経過時間[sec]を返す
auto Timer::GetElapsed() const noexcept -> SecondsType
{
    using namespace std::chrono;
    auto beg = begin_;
    auto now = Now();
    return ToSeconds<SecondsType>(now - beg);
}

// 経過時間[sec]を返すとともにタイマーをリセットする
auto Timer::GetElapsedAndReset() noexcept -> SecondsType
{
    using namespace std::chrono;
    auto beg = begin_;
    auto now = Now();
    new(this) Timer(now); // time_point にコピーが無いので new(place) で代用
    return ToSeconds<SecondsType>(now - beg);
}

// 整数経過時間[sec]を返すとともにタイマーをリセットする
auto Timer::GetIntegerElapsedAndReset() noexcept -> IntegerSecondsType
{
    using namespace std::chrono;
    auto beg = begin_;
    auto now = Now();
    auto elp = now - beg;
    if (auto sec = elp / seconds(1)) {
        new(this) Timer(beg + seconds(sec)); // time_point にコピーが無いので new(place) で代用
        return sec;
    }
    return 0;
}

// タイマーをリセットする
void Timer::Reset() noexcept
{
    new(this) Timer(); // time_point にコピーが無いので new(place) で代用
}

/*
 *
 */

} // namespace Pits

/*
 *
 */

// TODO : GetThreadTimes FILETIME による user kern 時間計測
