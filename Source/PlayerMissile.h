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

class PlayerMissile : public LogicComponent
{
public:
	PlayerMissile(Context*);

	void Initialise(ResourceCache* cache, Scene* scene);

	void Shoot(Vector3 spawnPos, Vector3 dir);

	void Update(float delta);

	Vector3 GetPosition();

private:
	Node* node;
	RigidBody* rigidBody;
	CollisionShape* collisionShape;
	StaticModel* staticModel;

	float speed = 20.0f;
};