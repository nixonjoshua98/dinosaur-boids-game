
#include <Urho3D/Graphics/DebugRenderer.h>

#include "Boid.h"

#include "BoidManager.h"

BoidManager::~BoidManager()
{
	Stop();

	thread.join();

	for (int i = 0; i < NUM_BOIDS; i++)
		delete boids[i];

	delete[] boids;
}

void BoidManager::Initialise(ResourceCache* cache, Scene* _scene)
{
	// Create the base neighbour lookup table
	for (float i = FLOOR_SIZE / 2; i < FLOOR_SIZE * 2; i += CELL_SIZE)
	{
		for (float j = FLOOR_SIZE / 2; j < FLOOR_SIZE * 2; j += CELL_SIZE)
		{
			boidNeighbourMap.emplace(GetCellKey({ i, 1.0f, j }), std::vector<Boid*>());
		}
	}

	boids = new Boid * [NUM_BOIDS];

	for (int i = 0; i < NUM_BOIDS; i++)
	{
		boids[i] = new Boid();

		boids[i]->Initialise(cache, _scene);
	}

	scene = _scene;

	threads = std::vector<std::thread>(NUM_THREADS);

	thread = std::thread(&BoidManager::UpdateThread, this);
}

void BoidManager::SetDeltaTime(float delta)
{
	deltaTime = delta;
}

void BoidManager::Stop()
{
	isRunning = false;
}

void BoidManager::Update()
{
	auto indexes = GetUpdateIndexes();

	UpdateNeighbourMap();

	for (int i = indexes.first; i < indexes.second; i++)
	{
		auto key = GetCellKey(boids[i]->GetPosition());

		if (boidNeighbourMap[key].size() == 0)
			continue;

		boids[i]->ComputeForce(boidNeighbourMap[key]);

		boids[i]->Update(deltaTime);
	}
}

void BoidManager::UpdateThread()
{
	while (isRunning)
	{
		Update();
	}
}

void BoidManager::UpdateNeighbourMap()
{
	auto it = boidNeighbourMap.begin();

	// Clear
	while (it != boidNeighbourMap.end())
	{
		it->second.clear();

		it++;
	}

	// Update
	for (int i = 0; i < NUM_BOIDS; i++)
	{
		std::pair<int, int> key = GetCellKey(boids[i]->GetPosition());

		boidNeighbourMap[key].push_back(boids[i]);
	}
}

std::pair<int, int> BoidManager::GetUpdateIndexes()
{
	int start, end;

	switch (alternateUpdate)
	{

	case BoidsUpdateEnum::FIRST_HALF:
		alternateUpdate = BoidsUpdateEnum::SECOND_HALF;

		start = 0;
		end = NUM_BOIDS / 2;
		break;

	case BoidsUpdateEnum::SECOND_HALF:
		alternateUpdate = BoidsUpdateEnum::FIRST_HALF;

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

	return { start, end };
}

inline std::pair<int, int> BoidManager::GetCellKey(Vector3 v)
{
	int x_tile = v.x_ + (FLOOR_SIZE / 2);
	int z_tile = v.z_ + (FLOOR_SIZE / 2);

	int x_index = (x_tile - (x_tile % CELL_SIZE)) / CELL_SIZE;
	int z_index = (z_tile - (z_tile % CELL_SIZE)) / CELL_SIZE;

	return { x_index, z_index };
}
