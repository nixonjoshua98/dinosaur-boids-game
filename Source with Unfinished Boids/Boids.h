
#pragma once

#include "UrhoHeaders.h"

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
	void ComputeForce(Boid*);
	void Update(float);


private:
	Vector3 force;

	Node* pNode;
	RigidBody* pRigidBody;
	CollisionShape* PCollisionShape;
	StaticModel* pStaticModel;
};