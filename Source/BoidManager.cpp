
#include <Urho3D/Graphics/DebugRenderer.h>

#include "Boid.h"

#include "BoidManager.h"
#include "RealTimer.h"

BoidManager::BoidManager()
{
	boids = new Boid * [MAX_NUM_BOIDS];
}

BoidManager::~BoidManager()
{
	Stop();

	for (std::thread& t : threads)
	{
		if (t.joinable())
			t.join();
	}

	for (int i = 0; i < numBoids; i++)
		delete boids[i];

	delete[] boids;
}

void BoidManager::Initialise(ResourceCache* _cache, Scene* _scene)
{
	scene = _scene;
	cache = _cache;

	// Create the base neighbour lookup table
	for (float x = -FLOOR_SIZE / 2; x <= FLOOR_SIZE / 2; x += CELL_SIZE)
	{
		for (float z = -FLOOR_SIZE / 2; z <= FLOOR_SIZE / 2; z += CELL_SIZE)
		{
			boidNeighbourMap.emplace(GetCellKey({ x, 0.0f, z }), std::vector<Boid*>());			
		}
	}

	threads	= std::vector<std::thread>(NUM_BOID_THREADS);

	for (auto i = 0; i < threads.size(); i++)
		threads[i] = std::thread(&BoidManager::UpdateThread, this, i);
}

void BoidManager::Stop()
{
	isRunning = false;
}

void BoidManager::Update(float delta)
{
	deltaTime = delta;

	SpawnBoid(Max(1, delta * 250));

	UpdateNeighbourMap();

	currentFrame++;
}

std::vector<Boid*> BoidManager::GetBoidsInCell(Vector3 pos)
{
	auto key = GetCellKey(pos);

	return boidNeighbourMap[key];
}

void BoidManager::SpawnBoid(int amount)
{
	for (int i = 0; i < amount; i++)
	{
		if (numBoids < MAX_NUM_BOIDS)
		{
			boids[numBoids] = new Boid();

			boids[numBoids]->Initialise(cache, scene);

			numBoids++;
		}
	}
}

void BoidManager::UpdateThread(int threadID)
{
	unsigned int frame = currentFrame;

	while (isRunning)
	{
		// Syncs the update to a single frame
		if (frame != currentFrame)
		{
			frame = currentFrame;

			auto indexes = GetUpdateIndexes(threadID);

			for (int i = indexes.first; i < indexes.second; i++)
			{
				lock.lock();

				std::vector<Boid*> neighbours = GetBoidsInCell(boids[i]->GetPosition());

				lock.unlock();

				if (neighbours.size() == 0)
					continue;

				boids[i]->ComputeForce(neighbours);

				boids[i]->Update(deltaTime);
			}
		}
	}
}

void BoidManager::UpdateNeighbourMap()
{
	lock.lock();

	auto it = boidNeighbourMap.begin();

	// Clear
	while (it != boidNeighbourMap.end())
	{
		it->second.clear();

		it++;
	}

	destroyedBoids = 0;

	// Update
	for (int i = 0; i < numBoids; i++)
	{
		std::pair<int, int> key = GetCellKey(boids[i]->GetPosition());

		boidNeighbourMap[key].push_back(boids[i]);

		if (!boids[i]->IsEnabled())
			destroyedBoids++;
	}

	lock.unlock();
}

std::pair<int, int> BoidManager::GetUpdateIndexes(int threadID)
{

	int chunk_size = numBoids / NUM_BOID_THREADS;

	return { chunk_size * threadID, chunk_size * (threadID + 1) };

}

std::pair<int, int> BoidManager::GetCellKey(Vector3 v)
{
	int x_tile = v.x_ + (FLOOR_SIZE / 2);
	int z_tile = v.z_ + (FLOOR_SIZE / 2);

	int x_index = (x_tile - (x_tile % CELL_SIZE)) / CELL_SIZE;
	int z_index = (z_tile - (z_tile % CELL_SIZE)) / CELL_SIZE;

	return { x_index, z_index };
}