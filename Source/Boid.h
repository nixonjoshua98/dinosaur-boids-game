
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

	static float Range_FAttract;
	static float Range_FRepel;
	static float Range_FAlign;
	static float FAttract_Factor;
	static float FRepel_Factor;
	static float FAlign_Factor;
	static float FAttract_Vmax;

	void Initialise(ResourceCache*, Scene*);
	void CreateComponents(ResourceCache*, Scene*);

	void Update(float);

	void ComputeForce(std::vector<Boid*>);

	Vector3 GetPosition();
	Vector3 GetLinearVelocity();

private:
	Vector3 force;

	Node* node;
	RigidBody* rigidBody;
	CollisionShape* collisionShape;
	StaticModel* staticModel;
};