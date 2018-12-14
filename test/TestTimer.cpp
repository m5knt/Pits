#include "../src/Timer.hpp"
#include <cstdlib>
#include <cassert>
#include <thread>

int main() {
    auto timer = Pits::Timer {};
    std::this_thread::sleep_for(std::chrono::seconds(1));
    assert(timer.GetElapsed() >= 1.0);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    assert(timer.GetElapsed() >= 2.0);

    timer.Reset();
    auto t = double {};
    std::this_thread::sleep_for(std::chrono::seconds(1));
    t = timer.GetElapsedAndReset();
    assert(t >= 1.0);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    t = timer.GetElapsedAndReset();
    assert(t >= 1.0);

	// 応答性が悪いとテスト失敗します
    timer.Reset();
    auto i = std::int64_t {};
    std::this_thread::sleep_for(std::chrono::milliseconds(1500));
    i = timer.GetIntegerElapsedAndReset();
    assert(i == 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    i = timer.GetIntegerElapsedAndReset();
    assert(i == 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(0));
    i = timer.GetIntegerElapsedAndReset();
    assert(i == 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    i = timer.GetIntegerElapsedAndReset();
    assert(i == 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    i = timer.GetIntegerElapsedAndReset();
    assert(i == 1);

	t = Pits::ToSeconds<float>(std::chrono::milliseconds(1500));
	assert(t == 1.5f);
	t = Pits::ToSeconds<double>(std::chrono::milliseconds(1500));
	assert(t == 1.5f);
	i = Pits::ToSeconds<std::int64_t>(std::chrono::milliseconds(1500));
	assert(i == 1);

    return EXIT_SUCCESS;
}
