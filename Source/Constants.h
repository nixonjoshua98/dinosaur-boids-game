
#pragma once

#include <thread>

const int FLOOR_SIZE		= 1024;
const int CELL_SIZE			= 64;
const int MAX_NUM_BOIDS		= 100;
const int NUM_BOID_THREADS	= 1;	// Clamp(1, 1, (int) std::thread::hardware_concurrency);