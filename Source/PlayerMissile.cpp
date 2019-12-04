
#include <Urho3D/Resource/ResourceCache.h>

#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/Node.h>

#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Physics/CollisionShape.h>

#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Camera.h>

#include <string>
#include <iostream>

#include "PlayerMissile.h"

void PlayerMissile::Initialise(ResourceCache* cache, Scene* scene)
{
	node = scene->CreateChild("Missile");

	staticModel			= node->CreateComponent<StaticModel>();
	rigidBody			= node->CreateComponent<RigidBody>();
	collisionShape		= node->CreateComponent<CollisionShape>();

	staticModel->SetModel(cache->GetResource<Model>("Models/Sphere.mdl"));
	staticModel->SetMaterial(cache->GetResource<Material>("Materials/Stone.xml"));

	node->SetScale(0.5f);

	rigidBody->SetMass(1.0f);
	rigidBody->SetUseGravity(false);
	rigidBody->SetPosition({ 0.0f, 0.0f, 0.0f });

	node->SetEnabled(false);
}

void PlayerMissile::Shoot(Vector3 spawnPos, Vector3 dir)
{
	if (lifetime <= 0.0f)
	{
		Vector3 v = spawnPos + (dir * 3);

		node->SetEnabled(true);

		lifetime = 1.0f;

		v.y_ = 1.f;

		rigidBody->SetPosition(v);

		rigidBody->SetLinearVelocity(dir.Normalized() * speed);
	}
}

void PlayerMissile::Update(float delta)
{
	lifetime -= delta;

	if (!IsEnabled())
		return;

	if (lifetime <= 0.0f)
	{
		node->SetEnabled(false);
	}
	else
	{
		Vector3 v = rigidBody->GetLinearVelocity();

		v.y_ = 0.0f;

		rigidBody->SetLinearVelocity(v);
	}
}

bool PlayerMissile::IsEnabled()
{
	return node->IsEnabled();
}

void PlayerMissile::Disable()
{
	lifetime = 0.0f;
	node->SetEnabled(false);
}

Vector3 PlayerMissile::GetPosition()
{
	return rigidBody->GetPosition();
}
