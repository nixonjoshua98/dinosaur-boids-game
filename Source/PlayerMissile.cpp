
#include <Urho3D/Resource/ResourceCache.h>

#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/Node.h>

#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Physics/CollisionShape.h>

#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Camera.h>

#include "PlayerMissile.h"

void PlayerMissile::Init(ResourceCache* cache, Scene* scene)
{
	node = scene->CreateChild("Missile");

	staticModel		= node->CreateComponent<StaticModel>();
	rigidBody		= node->CreateComponent<RigidBody>();
	collisionShape	= node->CreateComponent<CollisionShape>();

	staticModel->SetModel(cache->GetResource<Model>("Models/Cylinder.mdl"));
	staticModel->SetMaterial(cache->GetResource<Material>("Materials/Stone.xml"));

	node->SetScale(2.5f);

	rigidBody->SetMass(1.0f);
	rigidBody->SetUseGravity(false);

	collisionShape->SetBox(Vector3::ONE);

	rigidBody->SetPosition({ 0.0f, -5.0f, 0.0f });
}

void PlayerMissile::Fire(Node* camera)
{

	rigidBody->SetPosition(camera->GetPosition());
	rigidBody->SetLinearVelocity(camera->GetDirection().Normalized() * speed);
}
