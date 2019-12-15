
#pragma once

#include <vector>
#include <iostream>

#include <Urho3D/Scene/Node.h>

namespace Urho3D
{
	class ResourceCache;

	class RigidBody;
	class CollisionShape;
	class StaticModel;
	class Scene;
	class Vector3;
}

using namespace Urho3D;

class Boid
{
public:

	static float ATTRACT_RANGE;
	static float REPEL_RANGE;
	static float ALLIGN_RANGE;
	static float ATTRACT_STRENGTH;
	static float REPEL_STRENGTH;
	static float ALIGN_STRENGTH;
	static float ATTRACT_MAX_V;

	void Initialise(ResourceCache*, Scene*);
	void CreateComponents(ResourceCache*, Scene*);

	void Update(float);

	void ComputeForce(std::vector<Boid*> boids);

	bool IsEnabled();

	void Destroy();

	Vector3 GetPosition();
	Vector3 GetLinearVelocity();

private:
	Vector3 force;

	Node* node;
	RigidBody* rigidBody;
	StaticModel* staticModel;
};