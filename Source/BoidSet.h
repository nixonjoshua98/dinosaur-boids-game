#include "UrhoHeaders.h"
#include "Boid.h"

class BoidSet
{
public:
	~BoidSet();

	Boid* boidArray;	// Deleted in deconstructor

	void Initialise(ResourceCache*, Scene*);

	void Update(float tm);

private:
	const int NUM_BOIDS = 100;
};