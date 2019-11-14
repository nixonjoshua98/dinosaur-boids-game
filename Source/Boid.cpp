
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/DebugRenderer.h>

#include <Urho3D/Scene/Scene.h>

#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Physics/CollisionShape.h>

#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/StaticModel.h>

#include <iostream>

#include "Boid.h"
#include "Constants.h"

#define PRINT_V3(v) std::cout << "(" << v.x_ << ", " << v.y_ << ", " << v.z_ << ")\n";

float Boid::Range_FAttract	= 30.0f;
float Boid::Range_FRepel	= 30.0f;
float Boid::Range_FAlign		= 30.0f;

float Boid::FAlign_Factor = 4.0f;

float Boid::FAttract_Vmax = 6.0f;
float Boid::FAttract_Factor = 15.0f;

float Boid::FRepel_Factor = 5.0f;

void Boid::Initialise(ResourceCache* cache, Scene* scene)
{
	CreateComponents(cache, scene);
}

void Boid::CreateComponents(ResourceCache* cache, Scene* scene)
{
	node = scene->CreateChild("Boid");

	staticModel	= node->CreateComponent<StaticModel>();
	rigidBody		= node->CreateComponent<RigidBody>();

	staticModel->SetModel(cache->GetResource<Model>("Models/Cone.mdl"));
	staticModel->SetMaterial(cache->GetResource<Material>("Materials/Stone.xml"));

	node->SetScale(1.0f);

	rigidBody->SetMass(1.0f);
	rigidBody->SetUseGravity(false);

	rigidBody->SetPosition({Random(-10.0f, 10.0f), 0.5f, Random(-10.0f, 10.0f) });
	rigidBody->SetLinearVelocity({ Random(-10.0f, 10.0f), 0.0f, Random(-10.0f, 10.0f) });
}							   

void Boid::Update(float tm)
{
	if (!node->IsEnabled()) return;

	rigidBody->ApplyForce(force);

	Vector3 vel = GetLinearVelocity();

	vel.y_ = 0.0f;

	float d = Clamp(vel.Length(), 10.0f, 50.0f);

	rigidBody->SetLinearVelocity(vel.Normalized() * d);

	// Rotation	   
	Vector3 vn			= vel.Normalized();
	Vector3 cp			= -vn.CrossProduct(Vector3(0.0f, 1.0f, 0.0f));
	float dp				= cp.DotProduct(vn);

	rigidBody->SetRotation(Quaternion(Acos(dp), cp));
}

void Boid::ComputeForce(std::vector<Boid*> boids)
{
	if (!node->IsEnabled()) return;

	Vector3 CoM;				// Centre of mass, accumulated total

	int attractNeighbours		= 0;
	int allignNeighbours		= 0;

	Vector3 avgVelocity;
	Vector3 sepV;

	force = Vector3(0, 0, 0);		// Set the force member variable to zero

	for (unsigned int i = 0; i < boids.size(); i++)
	{
		Vector3 otherBoidPosition = boids[i]->GetPosition();
		Vector3 otherBoidVelocity = boids[i]->GetLinearVelocity();

		// Ignore itself
		if (GetPosition() == otherBoidPosition)
			continue;
		
		Vector3 sep = GetPosition() - otherBoidPosition;

		if (sep.Length() < Range_FAttract)
		{
			CoM += otherBoidPosition;
			attractNeighbours++;
		}
		
		if (sep.Length() < Range_FRepel)
		{
			sepV += sep.Normalized();
			
		}

		if (sep.Length() < Range_FAlign)
		{
			avgVelocity += otherBoidVelocity;
			allignNeighbours++;
		}
	}

	if (attractNeighbours > 0)
	{
		Vector3 desiredDirection		= ( (CoM  / attractNeighbours ) - GetPosition()).Normalized();
		Vector3 desiredVelocity		= desiredDirection * FAttract_Vmax;

		// Forces
		Vector3 attractive		= (desiredVelocity - GetLinearVelocity()) * FAttract_Factor;
		Vector3 allignment		= ( (avgVelocity / allignNeighbours) - GetLinearVelocity()) * FAlign_Factor;
		Vector3 repel				= sepV * FRepel_Factor;

		force = (attractive + allignment + repel);
	}
}

bool Boid::IsEnabled()
{
	return node->IsEnabled();
}

void Boid::Destroy()
{
	node->SetEnabled(false);
}

Vector3 Boid::GetPosition()
{
	return rigidBody->GetPosition();
}

Vector3 Boid::GetLinearVelocity()
{
	return rigidBody->GetLinearVelocity();
}
