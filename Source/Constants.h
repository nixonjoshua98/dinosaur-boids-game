
#pragma once

const int FLOOR_SIZE		= 1024;
const int CELL_SIZE			= 32;
const int MAX_NUM_BOIDS		= 100;
const int NUM_BOID_THREADS	= 1;

#define PRINT_V3(v) std::cout << "(" << v.x_ << ", " << v.y_ << ", " << v.z_ << ")\n";