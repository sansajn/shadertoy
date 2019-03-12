#include <cmath>
#include "clock.hpp"

using std::ceil;

universe_clock::universe_clock()
	: _now{0.0f}
{
	_t0 = std::chrono::system_clock::now();
}

float universe_clock::now() const
{
	return _now;
}

float universe_clock::next()
{
	std::chrono::duration<double> dt = std::chrono::system_clock::now() - _t0;
	_now = (float)(dt.count());
	return _now;
}

float universe_clock::next(float step)
{
	_now += step;
	return _now;
}


void universe_clock::resume()
{
	auto offset = std::chrono::milliseconds{static_cast<unsigned>(
		ceil(_now * 1000.0f))};

	_t0 = std::chrono::system_clock::now() - offset;
}
