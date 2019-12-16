
#pragma once

#include <chrono>
#include <string>

class RealTimer
{
public:
	RealTimer(std::string = "Timer: ");
	~RealTimer();

	float Tick();
	void Reset();

private:
	std::string txt;

	std::chrono::system_clock::time_point start;
};
