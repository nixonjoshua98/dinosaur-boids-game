

#include "Boid.h"

#include <vector>
#include <unordered_map>
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

	const int NUM_BOIDS = 650;

	BoidsUpdateEnum updateBoids = BoidsUpdateEnum::ALL;

	// Cell lookup table
	std::unordered_map< std::string, std::vector<Boid> > boidTable;

	// Position hashing
	int floorSize;
	int cellSize;
	std::string GetPositionHash(Vector3);
	//

	Boid* boids;
};