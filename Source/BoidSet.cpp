#include "BoidSet.h"

BoidSet::~BoidSet()
{
	delete[] boidArray;
}

void BoidSet::Initialise(ResourceCache* cache, Scene* scene)
{
	boidArray = new Boid[NUM_BOIDS];

	for (int i = 0; i < NUM_BOIDS; i++)
	{
		boidArray[i].Initialise(cache, scene);
	}
}

void BoidSet::Update(float tm)
{
	for (int i = 0; i < NUM_BOIDS; i++)
	{
		boidArray[i].ComputeForce(&boidArray[0], NUM_BOIDS, i == 0);

		boidArray[i].Update(tm);
	}
}
