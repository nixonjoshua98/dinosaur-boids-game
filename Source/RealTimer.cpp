#include "RealTimer.h"

#include <iostream>

// Default constructor
RealTimer::RealTimer(std::string _txt)
{
	txt = _txt;

	Reset();
}


RealTimer::~RealTimer()
{
	std::cout << txt << Tick() << "ms\n";
}

// Returns time since start in milliseconds
float RealTimer::Tick()
{
	auto end = std::chrono::system_clock::now();
	std::chrono::duration<float, std::milli> span = end - start;

	return span.count();
}

void RealTimer::Reset()
{
	start = std::chrono::system_clock::now();
}