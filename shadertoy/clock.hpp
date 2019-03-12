#pragma once
#include <chrono>

class universe_clock
{
public:
	universe_clock();
	float now() const;
	float next();
	float next(float step);
	void resume();

private:
	float _now;
	std::chrono::system_clock::time_point _t0;
};
