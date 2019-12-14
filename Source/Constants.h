
#pragma once

const int FLOOR_SIZE		= 4096;
const int CELL_SIZE			= 16;
const int MAX_NUM_BOIDS		= 1;
const int NUM_BOID_THREADS	= 1;

template<typename T>
constexpr auto PRINT_V3(T v) { return std::cout << "(" << v.x_ << ", " << v.y_ << ", " << v.z_ << ")\n";; }