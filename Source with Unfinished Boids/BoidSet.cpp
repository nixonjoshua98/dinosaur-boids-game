#include "BoidSet.h"

void BoidSet::Initialise(ResourceCache* cache, Scene* scene)
{
	for (int i = 0; i < 100; i++)
	{
		boidList[i].Initialise(cache, scene);
	}
}

void BoidSet::Update(float tm)
{
	for (int i = 0; i < 100; i++)
	{
		boidList[i].ComputeForce(&boidList[0]);

		boidList[i].Update(tm);
	}
}
