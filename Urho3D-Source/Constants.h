
#pragma once

const int FLOOR_SIZE		= 1024;
const int CELL_SIZE			= 16;
const int NUM_BOID_THREADS	= 1;

const int MAX_ONLINE_NUM_BOIDS	= 250;
const int MAX_OFFLINE_NUM_BOIDS = 4500;

template<typename T>
constexpr auto PRINT_V3(T v) { return std::cout << "(" << v.x_ << ", " << v.y_ << ", " << v.z_ << ")\n";; }