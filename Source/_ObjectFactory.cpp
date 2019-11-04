

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


Scene* _ObjectFactory::CreateScene(Context* context)
{
	Scene* scene = new Scene(context);

	scene->CreateComponent<Octree>();
	scene->CreateComponent<PhysicsWorld>();

	return scene;
}

Camera* _ObjectFactory::SetupCamera(SharedPtr<Node> cameraNode, Context* context)
{
	Camera* camera = cameraNode->CreateComponent<Camera>();

	cameraNode->SetPosition(Vector3(0.0f, 5.0f, 0.0f));

	camera->SetFarClip(300.0f);

	return camera;
}

void _ObjectFactory::SetCache(ResourceCache* c)
{
	cache = c;
}

void _ObjectFactory::SetScene(Scene* s)
{
	scene = s;
}

Character* _ObjectFactory::CreateCharacter(String name, Vector3 pos)
{
	Node* objectNode = scene->CreateChild(name);
	objectNode->SetPosition(pos);

	// spin node
	Node* adjustNode = objectNode->CreateChild("AdjNode");
	adjustNode->SetRotation(Quaternion(180, Vector3(0, 1, 0)));

	// Create the rendering component + animation controller
	AnimatedModel* object = adjustNode->CreateComponent<AnimatedModel>();
	object->SetModel(cache->GetResource<Model>("Models/Mutant/Mutant.mdl"));
	object->SetMaterial(cache->GetResource<Material>("Models/Mutant/Materials/mutant_M.xml"));
	object->SetCastShadows(true);
	adjustNode->CreateComponent<AnimationController>();

	// Set the head bone for manual control
	object->GetSkeleton().GetBone("Mutant:Head")->animated_ = false;

	// Create rigidbody, and set non-zero mass so that the body becomes dynamic
	RigidBody* body = objectNode->CreateComponent<RigidBody>();
	body->SetCollisionLayer(1);
	body->SetMass(1.0f);

	// Set zero angular factor so that physics doesn't turn the character on its own.
	// Instead we will control the character yaw manually
	body->SetAngularFactor(Vector3::ZERO);

	// Set the rigidbody to signal collision also when in rest, so that we get ground collisions properly
	body->SetCollisionEventMode(COLLISION_ALWAYS);

	// Set a capsule shape for collision
	CollisionShape* shape = objectNode->CreateComponent<CollisionShape>();
	shape->SetCapsule(0.7f, 1.8f, Vector3(0.0f, 0.9f, 0.0f));

	// Create the character logic component, which takes care of steering the rigidbody
	// Remember it so that we can set the controls. Use a WeakPtr because the scene hierarchy already owns it
	// and keeps it alive as long as it's not removed from the hierarchy
	return objectNode->CreateComponent<Character>();
}

void _ObjectFactory::CreateZone()
{
	Node* zoneNode = scene->CreateChild("Zone");

	Zone* zone = zoneNode->CreateComponent<Zone>();

	zone->SetAmbientColor(Color(0.15f, 0.15f, 0.15f));
	zone->SetFogColor(Color(0.5f, 0.5f, 0.7f));
	zone->SetBoundingBox(BoundingBox(-FLOOR_SIZE, FLOOR_SIZE));
}

void _ObjectFactory::CreateLight(LightType lightType)
{
	Node* lightNode = scene->CreateChild("DirectionalLight");

	lightNode->SetDirection(Vector3(0.3f, -0.5f, 0.425f));

	Light* light = lightNode->CreateComponent<Light>();

	light->SetLightType(lightType);
	light->SetCastShadows(true);
	light->SetShadowBias(BiasParameters(0.00025f, 0.5f));
	light->SetShadowCascade(CascadeParameters(10.0f, 50.0f, 200.0f, 0.0f, 0.8f));
	light->SetSpecularIntensity(0.5f);
}

void _ObjectFactory::CreateFloor()
{
	Node* floorNode = scene->CreateChild("Floor");
	StaticModel* model = floorNode->CreateComponent<StaticModel>();
	RigidBody* body = floorNode->CreateComponent<RigidBody>();

	floorNode->SetPosition(Vector3(0.0f, -0.5f, 0.0f));
	floorNode->SetScale(Vector3(FLOOR_SIZE, 1.0f, FLOOR_SIZE));

	model->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
	model->SetMaterial(cache->GetResource<Material>("Materials/Stone.xml"));

	body->SetCollisionLayer(2);

	CollisionShape* shape = floorNode->CreateComponent<CollisionShape>();

	shape->SetBox(Vector3::ONE);
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
