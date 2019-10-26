

#include <Urho3D/Resource/ResourceCache.h>

#include <Urho3D/Scene/Scene.h>

#include <chrono>
#include <iostream>

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
			boidTable.emplace(GetCellKey({ i, 1.0f, j }), std::vector<Boid>());
		}
	}

	for (int i = 0; i < NUM_BOIDS; i++)
	{
		boids[i].Initialise(cache, scene);
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

	auto keys = UpdateBoidTable();

	for (int i = start; i < end; i++)
	{
		boids[i].ComputeForceUsingTable(boidTable[keys[i]]);

		boids[i].Update(tm);
	}
}

std::vector< std::pair<int, int> > BoidSet::UpdateBoidTable()
{
	// Clear previous cells

	auto it = boidTable.begin();

	while (it != boidTable.end())
	{
		it->second.clear();

		it++;
	}

	auto keys = std::vector< std::pair<int, int> >(NUM_BOIDS);

	// Update new cells
	for (int i = 0; i < NUM_BOIDS; i++)
	{
		std::pair<int, int> key = GetCellKey(boids[i].GetPosition());

		keys[i] = key;

		boidTable[key].push_back(boids[i]);
	}

	return keys;
}

std::pair<int, int> BoidSet::GetCellKey(Vector3 v)
{
	int x_tile = v.x_ + (floorSize / 2);
	int z_tile = v.z_ + (floorSize / 2);
	
	int x_index = (x_tile - (x_tile % cellSize)) / cellSize;
	int z_index = (z_tile - (z_tile % cellSize)) / cellSize;
	
	return { x_index, z_index };
}
