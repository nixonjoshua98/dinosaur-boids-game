#pragma once

#include <Urho3D/Scene/LogicComponent.h>

namespace Urho3D
{
	class ResourceCache;
	class RigidBody;
	class CollisionShape;
	class StaticModel;
	class Scene;
}

using namespace Urho3D;

class PlayerMissile
{
public:
	float lifetime = 0.0f;

	void Initialise(ResourceCache* cache, Scene* scene);

	void Shoot(Vector3 spawnPos, Vector3 dir);

	void Update(float delta);

	bool IsEnabled();

	void Disable();

	Vector3 GetPosition();

private:
	Node* node;
	RigidBody* rigidBody;
	CollisionShape* collisionShape;
	StaticModel* staticModel;

	float speed = 100.0f;
};