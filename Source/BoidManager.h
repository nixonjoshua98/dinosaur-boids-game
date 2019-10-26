
#pragma once

#include <Urho3D/Math/Vector3.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>

#include <vector>
#include <map>
#include <iostream>
#include <thread>

#include "Constants.h"

using namespace Urho3D;

class Boid;

enum class BoidsUpdateEnum
{
	ALL,
	FIRST_HALF,
	SECOND_HALF
};

class BoidManager
{
public:

	~BoidManager();
	
	void Initialise(ResourceCache*, Scene* scene);

	void Stop();

	void Update(float);

private:
	Boid** boids;

	BoidsUpdateEnum alternateUpdate = BoidsUpdateEnum::FIRST_HALF;

	std::map< std::pair<int, int>, std::vector<Boid*> > boidNeighbourMap;

	std::thread thread;

	Scene* scene;

	bool isRunning = true;

	float deltaTime = 0.0f;

	void UpdateThread();

	void UpdateNeighbourMap();

	std::pair<int, int> GetUpdateIndexes();
	std::pair<int, int> GetCellKey(Vector3 pos);
};