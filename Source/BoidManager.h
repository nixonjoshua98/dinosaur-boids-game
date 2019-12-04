
#pragma once

#include <Urho3D/Math/Vector3.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>

#include <vector>
#include <map>
#include <iostream>
#include <mutex>
#include <thread>

#include "Constants.h"

using namespace Urho3D;

class Boid;

class BoidManager
{
public:
	BoidManager();
	~BoidManager();

	void Initialise();
	
	void Initialise(ResourceCache*, Scene* scene);

	void Stop();

	void Update(float);

	unsigned int GetNumBoids() { return numBoids; }	  
	unsigned int GetNumEnabledBoids() { return numBoids - destroyedBoids; }

	Boid** GetBoids() { return boids; }

	std::vector<Boid*> GetBoidsInCell(Vector3 pos);
	std::vector<Boid*> GetBoidsInCell(std::pair<int, int> key);

private:
	Boid** boids;

	bool isUpdatingBoids;

	std::map< std::pair<int, int>, std::vector<Boid*> > boidNeighbourMap;

	std::vector<std::thread> threads;
	std::mutex lock;

	Scene*			scene;
	ResourceCache*	cache;

	bool isRunning		= true;
	float deltaTime		= 0.0f;

	unsigned int currentFrame	= -1;
	unsigned int numBoids		= 0;
	unsigned int destroyedBoids = 0;

	void SpawnBoid(int);
	void UpdateThread(int);
	void UpdateNeighbourMap();

	std::vector<Boid*> GetBoidsInNeighbouringCells(Vector3 pos);

	std::pair<int, int> GetUpdateIndexes(int);
	std::pair<int, int> GetCellKey(Vector3 pos);
};