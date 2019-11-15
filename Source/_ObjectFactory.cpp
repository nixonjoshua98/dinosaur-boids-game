

#include <Urho3D/Graphics/AnimatedModel.h>
#include <Urho3D/Graphics/AnimationController.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Light.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Zone.h>

#include <Urho3D/Input/Controls.h>

#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Physics/RigidBody.h>

#include <Urho3D/Resource/ResourceCache.h>

#include <Urho3D/Scene/Scene.h>

#include "_ObjectFactory.h"
#include "Character.h"
#include "Constants.h"

void _ObjectFactory::SetCache(ResourceCache* c)
{
	cache = c;
}

void _ObjectFactory::SetScene(Scene* s)
{
	scene = s;
}

void _ObjectFactory::CreateMushroom(Vector3 pos, Quaternion rotation, float scale)
{
	Node* node = CreateObject("Mushroom", "Models/Mushroom.mdl", "Materials/Mushroom.xml", pos, rotation, scale, 0.0f);

	CollisionShape* shape = node->CreateComponent<CollisionShape>();

	StaticModel* object = node->GetComponent<StaticModel>();

	shape->SetTriangleMesh(object->GetModel(), 0);
}

void _ObjectFactory::CreateBox(Vector3 pos, Quaternion rotation, float scale)
{
	Node* node = CreateObject("Box", "Models/Box.mdl", "Materials/Stone.xml", pos, rotation, scale, scale * 2.0f);

	CollisionShape* shape = node->CreateComponent<CollisionShape>();

	shape->SetBox(Vector3::ONE);
}

Node* _ObjectFactory::CreateObject(String name, String model, String mat, 
	Vector3 pos, Quaternion rotation, float scale, float mass)
{
	Node* node = scene->CreateChild(name);

	node->SetPosition(pos);
	node->SetRotation(rotation);
	node->SetScale(scale);

	StaticModel* object = node->CreateComponent<StaticModel>();

	object->SetModel(cache->GetResource<Model>(model));
	object->SetMaterial(cache->GetResource<Material>(mat));
	object->SetCastShadows(true);

	RigidBody* body = node->CreateComponent<RigidBody>();

	body->SetCollisionLayer(2);

	body->SetMass(mass);

	return node;
}
