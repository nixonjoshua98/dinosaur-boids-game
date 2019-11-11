
#include <Urho3D/Resource/ResourceCache.h>

#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/Node.h>

#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Physics/CollisionShape.h>

#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Camera.h>

#include <Urho3D/Physics/PhysicsEvents.h>

#include <string>
#include <iostream>

#include "PlayerMissile.h"


PlayerMissile::PlayerMissile(Context* context) : LogicComponent(context)
{

}

void PlayerMissile::Initialise(ResourceCache* cache, Scene* scene)
{
	node = scene->CreateChild("Missile");

	staticModel		= node->CreateComponent<StaticModel>();
	rigidBody		= node->CreateComponent<RigidBody>();
	collisionShape	= node->CreateComponent<CollisionShape>();

	staticModel->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
	staticModel->SetMaterial(cache->GetResource<Material>("Materials/Stone.xml"));

	node->SetScale(1.f);

	rigidBody->SetMass(1.0f);
	rigidBody->SetUseGravity(false);

	collisionShape->SetBox(Vector3::ONE);

	rigidBody->SetPosition({ 0.0f, -100.0f, 0.0f });
}

void PlayerMissile::Shoot(Vector3 spawnPos, Vector3 dir)
{

	Vector3 v = spawnPos + (dir * 3);

	//v.y_ = 1.f;

	rigidBody->SetPosition(v);

	rigidBody->SetLinearVelocity(dir.Normalized() * speed);

}

void PlayerMissile::Update(float delta)
{
	Vector3 v = rigidBody->GetLinearVelocity();

	v.y_ = 0.0f;

	rigidBody->SetLinearVelocity(v);
}

Vector3 PlayerMissile::GetPosition()
{
	return rigidBody->GetPosition();
}
