#include "Pits/Pits.hpp"
#include <cstdlib>
#include <cassert>
#include <thread>

int main() {

    assert(Pits::Pits.GetMultiByteEncoding() != Pits::StringLiteral::OTHER);
    assert(Pits::Pits.GetWideCharEncoding() != Pits::StringLiteral::OTHER);

    assert(Pits::Pits.IsMainThread());

	auto i = int {};
    std::this_thread::sleep_for(std::chrono::seconds(1));
	i = Pits::Pits.GetIntegerElapsed();
	assert(i == 1);
	Pits::Pits.Update();
	std::this_thread::sleep_for(std::chrono::seconds(2));
	i = Pits::Pits.GetIntegerElapsed();
	assert(i == 3);
    Pits::Pits.Update();
	i = Pits::Pits.GetRevisitMax();
    assert(i == 2);

    return EXIT_SUCCESS;
}
