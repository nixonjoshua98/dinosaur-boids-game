
#include "Boid.h"

float Boid::Range_FAttract = 30.0f;

float Boid::Range_FAlign = 3.0f;
float Boid::FAlign_Factor = 2.0f;

float Boid::FAttract_Vmax = 5.0f;
float Boid::FAttract_Factor = 4.0f;

float Boid::FRepel_Factor = 3.0f;
float Boid::Range_FRepel = 20.0f;

#include <Urho3D/Graphics/DebugRenderer.h>

void Boid::Initialise(ResourceCache* cache, Scene* scene)
{
	CreateComponents(cache, scene);
}

void Boid::CreateComponents(ResourceCache* cache, Scene* scene)
{
	node = scene->CreateChild("Boid");

	staticModel = node->CreateComponent<StaticModel>();
	rigidBody = node->CreateComponent<RigidBody>();
	collisionShape = node->CreateComponent<CollisionShape>();

	staticModel->SetModel(cache->GetResource<Model>("Models/Dinosaur.mdl"));

	rigidBody->SetMass(1.0f);
	rigidBody->SetUseGravity(false);
	rigidBody->SetPosition({ Random(180.0f) - 90.0f, 30.0f, Random(180.0f) - 90.0f });
	rigidBody->SetLinearVelocity({ Random(-20.0f, 20.0f), 0.0f, Random(-20.0f, 20.0f) });
}

void Boid::ComputeForce(Boid* boidArray, int totalBoids, bool debug)
{
	Vector3 CoM;				// Centre of mass, accumulated total
	int n = 0;					// Count number of neigbours	

	Vector3 avgVelocity;
	Vector3 sepV;

	force = Vector3(0, 0, 0);		// Set the force member variable to zero

	DebugRenderer* debugRenderer = node->GetScene()->GetComponent<DebugRenderer>();

	// Search Neighbourhood
	for (int i = 0; i < totalBoids; i++)
	{
		if (this == &boidArray[i])
			continue;

		Vector3 otherBoidPosition = boidArray[i].GetPosition();

		Vector3 sep = GetPosition() - otherBoidPosition;

		// Within range, so it is a neighbour
		if (sep.Length() < Range_FAttract)
		{
			CoM += otherBoidPosition;

			if (debug && debugRenderer)
				debugRenderer->AddLine(GetPosition(), otherBoidPosition, Color(1, 1, 1, 1), false);

			if (sep.Length() < Range_FRepel)
				sepV += sep.Normalized();

			n++;
		}

		if (sep.Length() < Range_FAlign)
			avgVelocity += boidArray[i].GetLinearVelocity();
	}

	if (n > 0)
	{
		CoM /= n;
		avgVelocity /= n;

		Vector3 dir = (CoM - GetPosition()).Normalized();		  
		Vector3 vDesired = dir * FAttract_Vmax;

		// Forces
		Vector3 attractive = (vDesired - GetLinearVelocity()) * FAttract_Factor;  
		Vector3 allignment = (avgVelocity - GetLinearVelocity()) * FAlign_Factor;		
		Vector3 repel = sepV * FRepel_Factor;

		force += (attractive + allignment + repel);
	}
}

void Boid::Update(float tm)
{
	rigidBody->ApplyForce(force);

	Vector3 vel = GetLinearVelocity();

	float d = Clamp(vel.Length(), 10.0f, 50.0f);

	rigidBody->SetLinearVelocity(vel.Normalized() * d);

	Vector3 vn		= vel.Normalized();

	Vector3 cp		= -vn.CrossProduct(Vector3(0.0f, 1.0f, 0.0f));

	float	dp			= cp.DotProduct(vn);

	//rigidBody->SetRotation(Quaternion(Acos(dp), cp));

	Vector3 currentPos = GetPosition();

	currentPos.y_ = 1.0f;

	rigidBody->SetPosition(currentPos);
}
