
#include <Urho3D/Graphics/DebugRenderer.h>

#include "Boid.h"

#include "BoidManager.h"
#include "RealTimer.h"

BoidManager::~BoidManager()
{
	Stop();

	for (std::thread& t : threads)
		t.join();

	for (int i = 0; i < numBoids; i++)
		delete boids[i];

	delete[] boids;
}

void BoidManager::Initialise(ResourceCache* _cache, Scene* _scene)
{
	auto timer = RealTimer("BoidManager::Initialise: ");

	scene = _scene;
	cache = _cache;

	// Create the base neighbour lookup table
	for (float i = FLOOR_SIZE / 2; i < FLOOR_SIZE * 2; i += CELL_SIZE)
	{
		for (float j = FLOOR_SIZE / 2; j < FLOOR_SIZE * 2; j += CELL_SIZE)
		{
			boidNeighbourMap.emplace(GetCellKey({ i, 0.0f, j }), std::vector<Boid*>());
		}
	}

	boids	= new Boid * [MAX_NUM_BOIDS];
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
				auto key = GetCellKey(boids[i]->GetPosition());

				lock.lock();

				bool hasNeighbours = boidNeighbourMap[key].size() == 0;

				lock.unlock();

				if (hasNeighbours)
					continue;

				boids[i]->ComputeForce(boidNeighbourMap[key]);

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

	// Update
	for (int i = 0; i < numBoids; i++)
	{
		std::pair<int, int> key = GetCellKey(boids[i]->GetPosition());

		boidNeighbourMap[key].push_back(boids[i]);
	}

	lock.unlock();
}

std::pair<int, int> BoidManager::GetUpdateIndexes(int threadID)
{
	int chunk_size = numBoids / NUM_BOID_THREADS;

	int start = 0, end = 0;

	switch (alternateUpdate)
	{

	case BoidsUpdateEnum::FIRST_HALF:
		alternateUpdate = BoidsUpdateEnum::SECOND_HALF;

		start	= threadID * (chunk_size / 2);
		end		= start + (chunk_size / 2);

		break;

	case BoidsUpdateEnum::SECOND_HALF:
		alternateUpdate = BoidsUpdateEnum::FIRST_HALF;

		start	= threadID * (chunk_size / 2) + numBoids / 2;
		end		= start + (chunk_size / 2);

		if (threadID + 1 == NUM_BOID_THREADS)
			end = numBoids;

		break;

	case BoidsUpdateEnum::ALL:
		start	= threadID * chunk_size;
		end		= start + chunk_size;

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