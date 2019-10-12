#include "UrhoHeaders.h"
#include "Boids.h"

class BoidSet
{
public:
	Boid boidList[100];

	BoidSet() {};

	void Initialise(ResourceCache* pRes, Scene* pScene);

	void Update(float tm);
};