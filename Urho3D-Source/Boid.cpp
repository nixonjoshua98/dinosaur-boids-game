
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/DebugRenderer.h>

#include <Urho3D/Scene/Scene.h>

#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Physics/CollisionShape.h>

#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/StaticModel.h>

#include <Urho3D/Graphics/ParticleEffect.h>
#include <Urho3D/Graphics/ParticleEmitter.h>

#include <iostream>

#include "Boid.h"
#include "Constants.h"



void Boid::Initialise(ResourceCache* cache, Scene* scene)
{
	CreateComponents(cache, scene);
}

void Boid::CreateComponents(ResourceCache* cache, Scene* scene)
{
	node = scene->CreateChild("Boid");

	staticModel	= node->CreateComponent<StaticModel>();
	rigidBody	= node->CreateComponent<RigidBody>();

	staticModel->SetModel(cache->GetResource<Model>("Models/Dinosaur.mdl"));
	staticModel->SetMaterial(cache->GetResource<Material>("Materials/Stone.xml"));

	node->SetScale(1.0f);

	rigidBody->SetMass(1.0f);
	rigidBody->SetUseGravity(false);

	rigidBody->SetPosition({Random(-100.0f, 100.0f), 0.5f, Random(-100.0f, 100.0f) });
}							   

void Boid::Update(float tm)
{
	if (!node->IsEnabled())
		return;

	// Death animation
	if (isDestroyed && node->IsEnabled())
	{
		Vector3 v = Vector3({ 0.0f, 5.0f, 0.0f });

		if (node != nullptr && node->IsEnabled())
		{
			rigidBody->SetLinearVelocity(v);
		}

		return;
	}

	force.y_ = 0.0f;

	rigidBody->ApplyForce(force);

	Vector3 vel = GetLinearVelocity();

	vel.y_ = 0.0f;

	float d = Clamp(vel.Length(), 10.0f, 50.0f);

	rigidBody->SetLinearVelocity(vel.Normalized() * d);

	// Rotation	   
	Vector3 vn	= vel.Normalized();
	Vector3 cp	= -vn.CrossProduct(Vector3(0.0f, 1.0f, 0.0f));
	float dp	= cp.DotProduct(vn);

	Quaternion rot = Quaternion(Acos(dp), cp);

	rot.y_ = rot.x_ + 0.25f;

	rot.x_ = 0.0f;
	rot.z_ = 0.0f;

	rigidBody->SetRotation(rot);
}

float Boid::ATTRACT_RANGE		= 20.0f;
float Boid::ATTRACT_STRENGTH	= 8.0f;
float Boid::ATTRACT_MAX_V		= 7.0f;

float Boid::REPEL_RANGE		= 15.0;
float Boid::REPEL_STRENGTH	= 12.0f;

float Boid::ALLIGN_RANGE	= 8.0f;
float Boid::ALIGN_STRENGTH	= 3.0f;

void Boid::ComputeForce(std::vector<Boid*> boids)
{
	if (isDestroyed)
		return;

	Vector3 CoM;				// Centre of mass, accumulated total

	int attractNeighbours	= 0;
	int allignNeighbours	= 0;
	int repelNeighbours		= 0;

	Vector3 totalVelocity;
	Vector3 totalVectorLength;

	force = Vector3(0, 0, 0);		// Set the force member variable to zero

	for (unsigned int i = 0; i < boids.size(); i++)
	{
		Vector3 otherBoidPosition = boids[i]->GetPosition();
		Vector3 otherBoidVelocity = boids[i]->GetLinearVelocity();

		// Ignore itself
		if (GetPosition() == otherBoidPosition || !boids[i]->IsEnabled())
			continue;
		
		Vector3 sep = GetPosition() - otherBoidPosition;

		if (sep.Length() < ATTRACT_RANGE)
		{
			CoM += otherBoidPosition;
			attractNeighbours++;
		}
		
		if (sep.Length() < REPEL_RANGE)
		{
			totalVectorLength += sep.Normalized();	
			repelNeighbours++;
		}

		if (sep.Length() < ALLIGN_RANGE)
		{
			totalVelocity += otherBoidVelocity;
			allignNeighbours++;
		}
	}

	// Attraction
	if (attractNeighbours > 0)
	{
		Vector3 desiredDirection = ((CoM / attractNeighbours) - GetPosition()).Normalized();
		Vector3 desiredVelocity = desiredDirection * ATTRACT_MAX_V;

		Vector3 attractionForce = (desiredVelocity - GetLinearVelocity()) * ATTRACT_STRENGTH;

		force += attractionForce;
	}

	// Center Attraction
	Vector3 desiredDirection = (Vector3::ZERO - GetPosition()).Normalized();
	Vector3 desiredVelocity = desiredDirection * 5.0f;

	Vector3 centerAttractForce = (desiredVelocity - GetLinearVelocity()) * 4.0f;

	force += centerAttractForce;

	// Repel
	if (repelNeighbours > 0)
	{
		Vector3 repelForce = totalVectorLength * REPEL_STRENGTH;

		force += repelForce;
	}

	// Allignment
	if (allignNeighbours > 0)
	{
		Vector3 allignment = ((totalVelocity / allignNeighbours) - GetLinearVelocity()).Normalized() * ALIGN_STRENGTH;

		force += allignment;
	}
}

bool Boid::IsEnabled()
{
	return !isDestroyed;
}

void Boid::DestroyIfNeeded()
{
	if (isDestroyed && GetPosition().y_ >= 100.0f)
	{
		node->SetEnabled(false);
	}
}

void Boid::Destroy()
{
	isDestroyed = true;
}

Vector3 Boid::GetPosition()
{
	return rigidBody->GetPosition();
}

Vector3 Boid::GetLinearVelocity()
{
	return rigidBody->GetLinearVelocity();
}
