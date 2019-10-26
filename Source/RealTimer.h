
#pragma once

#include <chrono>
#include <string>

class RealTimer
{
public:
	RealTimer();

	float Tick();
	void Reset();

protected:
	std::chrono::system_clock::time_point start;	// Time at init
};
