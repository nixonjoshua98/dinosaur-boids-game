

#include <Urho3D/Resource/ResourceCache.h>

#include <Urho3D/Scene/Scene.h>

#include "BoidSet.h"

void BoidSet::Initialise(int _floorSize, int _cellSize, ResourceCache* cache, Scene* scene)
{
	floorSize = _floorSize;
	cellSize = _cellSize;

	boids = new Boid[NUM_BOIDS];

	for (float i = floorSize / 2; i < floorSize * 2; i += cellSize)
	{
		for (float j = floorSize / 2; j < floorSize * 2; j += cellSize)
		{
			boidTable.emplace(GetPositionHash({ i, 1.0f, j }), std::vector<Boid>());
		}
	}

	for (int i = 0; i < NUM_BOIDS; i++)
	{
		boids[i].Initialise(cache, scene);

		std::string posHash = GetPositionHash(boids[i].GetPosition());

		boidTable[posHash].push_back(boids[i]);
	}
}

void BoidSet::Update(float tm)
{
	int start, end;

	// Optimisation
	switch (updateBoids)
	{

	case BoidsUpdateEnum::FIRST_HALF:
		updateBoids = BoidsUpdateEnum::SECOND_HALF;

		start = 0;
		end = NUM_BOIDS / 2;
		break;

	case BoidsUpdateEnum::SECOND_HALF:
		updateBoids = BoidsUpdateEnum::FIRST_HALF;

		start = NUM_BOIDS / 2;
		end = NUM_BOIDS;
		break;

	case BoidsUpdateEnum::ALL:
		start = 0;
		end = NUM_BOIDS;
		break;

	default:
		start = 0;
		end = NUM_BOIDS;
		break;

	}

	for (int i = start; i < end; i++)
	{
		boids[i].ComputeForceUsingTable(boidTable[GetPositionHash(boids[i].GetPosition())]);

		boids[i].Update(tm);
	}
}

std::string BoidSet::GetPositionHash(Vector3 v)
{
	int x_tile = v.x_ + (floorSize / 2);
	int z_tile = v.z_ + (floorSize / 2);
	
	int x_index = (x_tile - (x_tile % cellSize)) / cellSize;
	int z_index = (z_tile - (z_tile % cellSize)) / cellSize;
	
	return std::to_string(x_index) + std::to_string(z_index);
}
