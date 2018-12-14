/**
 * @brief 最低最悪土壺状態から壺を心得る事を目標としながら偏狭な陶芸家の様に壺を割るライブラリ "The PITS"
 * @author yukio kaneda
 * @file 
 */

#include "Pits.hpp"
#include <algorithm>    // min
#include <iostream>

/*
 *
 */

namespace Pits {

/*
 *
 */

class Pits Pits;

/*
 *
 */

Pits::~Pits()
{
    // グローバルデストラクタ動作中
    std::cout << "global static destoructor" << std::endl;
}

Pits::Pits()
    : main_thread_id_ {std::this_thread::get_id()}
    //, BeginSystemClock {std::chrono::system_clock::now()}
{
    // グローバルコンストラクタ動作中
    std::cout << "global static constructor" << std::endl;
    // std::cout << "multibyte string encoding : " << Pits::Standard::to_string(Pits::Pits.GetMultiByteEncoding()) << std::endl;
    // std::cout << "widechar string encoding : " << Pits::Standard::to_string(Pits::Pits.GetWideCharEncoding()) << std::endl;
    std::cout << "main threadid : " << main_thread_id_ << std::endl;
    //std::cout << "hardware concurrency : " << Pits::Pits.HardwareConCurrency << std::endl;
}

/*
 *
 */

auto Pits::GetMultiByteEncoding() const noexcept -> EncodingType
{
    return GetEncodingType("漢");
}

auto Pits::GetWideCharEncoding() const noexcept -> EncodingType
{
    return GetEncodingType(L"漢");
}

/*
 *
 */

auto Pits::Update() -> void
{
    revisit_max_ = std::max(revisit_max_, revisit_timer_.GetElapsedAndReset());
    elapsed_ += elapsed_timer_.GetIntegerElapsedAndReset();
}


auto Pits::GetRevisitMax() const noexcept -> double
{
    return revisit_max_;
}

auto Pits::GetIntegerElapsed() noexcept -> std::int64_t
{
    elapsed_ += elapsed_timer_.GetIntegerElapsedAndReset();
    return elapsed_;
}

// auto Pits::GetBeginTime() const noexcept -> std::time_t {
//     using namespace std::chrono;
//     auto now = system_clock::now();
//     auto dt = now - seconds(GetElapsed());
//     return dt;
// }

}

/*
 *
 */

// namespace PitsUser {

// // extern std::ostream* Out_ = 0;
// // extern std::ostream& Out {*Out_};

// }

/*
 *
 */

