
#pragma once

#include "UrhoHeaders.h"

using namespace Urho3D;

class Boid
{
public:
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

	void ComputeForce(Boid*, int, bool = false);
	void Update(float);

	Vector3 GetPosition()		{ return rigidBody->GetPosition();       }
	Vector3 GetLinearVelocity() { return rigidBody->GetLinearVelocity(); }

private:
	Vector3 force;

	Node* node;
	RigidBody* rigidBody;
	CollisionShape* collisionShape;
	StaticModel* staticModel;
};