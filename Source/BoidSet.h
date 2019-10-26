

#include "Boid.h"

#include <vector>
#include <map>
#include <thread>
#include <string>

class BoidSet
{
public:
	BoidSet() {};
	~BoidSet() { delete[] boids; };

	void Initialise(int, int, ResourceCache*, Scene*);

	void Update(float tm);

	int GetNumBoids() { return NUM_BOIDS; }

private:
	enum class BoidsUpdateEnum { ALL, FIRST_HALF, SECOND_HALF };

	const int NUM_BOIDS = 500;
	const int MAX_THREADS = std::thread::hardware_concurrency();

	BoidsUpdateEnum updateBoids = BoidsUpdateEnum::FIRST_HALF;
	std::map< std::pair<int, int>, std::vector<Boid> > boidTable;

	std::vector< std::pair<int, int> > UpdateBoidTable();

	// Position hashing
	int floorSize;
	int cellSize;
	std::pair<int, int> GetCellKey(Vector3);
	//

	Boid* boids;
};