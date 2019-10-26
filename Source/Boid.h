
#pragma once

#include <vector>

namespace Urho3D
{
	class ResourceCache;

	class RigidBody;
	class CollisionShape;
	class StaticModel;
}

using namespace Urho3D;

class Boid
{
public:
	Boid() {};
	~Boid() {};

	static float Range_FAttract;
	static float Range_FRepel;
	static float Range_FAlign;
	static float FAttract_Factor;
	static float FRepel_Factor;
	static float FAlign_Factor;
	static float FAttract_Vmax;

	void Initialise(ResourceCache*, Scene*);
	void CreateComponents(ResourceCache*, Scene*);

	void ComputeForce(Boid* boi, int);
	void Update(float);

	void ComputeForceUsingTable(std::vector<Boid>);

	Vector3 GetPosition();
	Vector3 GetLinearVelocity();

private:
	Vector3 force;

	Node* node;
	RigidBody* rigidBody;
	CollisionShape* collisionShape;
	StaticModel* staticModel;
};