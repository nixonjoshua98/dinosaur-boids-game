#include "RealTimer.h"

// Default constructor
RealTimer::RealTimer()
{
	Reset();
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