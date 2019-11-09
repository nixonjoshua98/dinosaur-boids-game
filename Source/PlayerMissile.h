#pragma once

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
	void Init(ResourceCache* cache, Scene* scene);
	void Fire(Node* camera);

private:
	Node* node;
	RigidBody* rigidBody;
	CollisionShape* collisionShape;
	StaticModel* staticModel;

	float speed;
};