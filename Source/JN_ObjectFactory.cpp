

#include "UrhoHeaders.h"

#include "JN_ObjectFactory.h"
#include "Character.h"


Scene* JN_ObjectFactory::CreateScene(Context* context)
{
	Scene* scene = new Scene(context);

	scene->CreateComponent<Octree>();
	scene->CreateComponent<PhysicsWorld>();

	return scene;
}

Camera* JN_ObjectFactory::SetupCamera(SharedPtr<Node> cameraNode, Context* context)
{
	Camera* camera = cameraNode->CreateComponent<Camera>();

	cameraNode->SetPosition(Vector3(0.0f, 5.0f, 0.0f));

	camera->SetFarClip(300.0f);

	return camera;
}

void JN_ObjectFactory::SetCache(ResourceCache* c)
{
	cache = c;
}

void JN_ObjectFactory::SetScene(Scene* s)
{
	scene = s;
}

void JN_ObjectFactory::SetRoot(UIElement* r)
{
	root = r;
}

Character* JN_ObjectFactory::CreateCharacter(String name, Vector3 pos)
{
	Node* objectNode = scene->CreateChild(name);
	AnimatedModel* object = objectNode->CreateComponent<AnimatedModel>();

	objectNode->SetPosition(pos);

	object->SetModel(cache->GetResource<Model>("Models/Jack.mdl"));
	object->SetMaterial(cache->GetResource<Material>("Materials/Jack.xml"));
	object->SetCastShadows(true);

	objectNode->CreateComponent<AnimationController>();

	object->GetSkeleton().GetBone("Bip01_Head")->animated_ = false;

	RigidBody* body = objectNode->CreateComponent<RigidBody>();
	CollisionShape* shape = objectNode->CreateComponent<CollisionShape>();

	body->SetCollisionLayer(1);
	body->SetMass(1.0f);

	// Set zero angular factor so that physics doesn't turn the character on its own.
	// Instead we will control the character yaw manually 
	body->SetAngularFactor(Vector3::ZERO);
	body->SetCollisionEventMode(COLLISION_ALWAYS);

	shape->SetCapsule(0.7f, 1.8f, Vector3(0.0f, 0.9f, 0.0f));

	return objectNode->CreateComponent<Character>();
}

void JN_ObjectFactory::CreateZone()
{
	Node* zoneNode = scene->CreateChild("Zone");

	Zone* zone = zoneNode->CreateComponent<Zone>();

	zone->SetAmbientColor(Color(0.15f, 0.15f, 0.15f));
	zone->SetFogColor(Color(0.5f, 0.5f, 0.7f));
	zone->SetFogStart(100.0f);
	zone->SetFogEnd(300.0f);
	zone->SetBoundingBox(BoundingBox(-1000.0f, 1000.0f));
}

void JN_ObjectFactory::CreateLight(LightType lightType)
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

void JN_ObjectFactory::CreateFloor()
{
	Node* floorNode = scene->CreateChild("Floor");
	StaticModel* model = floorNode->CreateComponent<StaticModel>();
	RigidBody* body = floorNode->CreateComponent<RigidBody>();

	floorNode->SetPosition(Vector3(0.0f, -0.5f, 0.0f));
	floorNode->SetScale(Vector3(200.0f, 1.0f, 200.0f));

	model->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
	model->SetMaterial(cache->GetResource<Material>("Materials/Stone.xml"));

	body->SetCollisionLayer(2);

	CollisionShape* shape = floorNode->CreateComponent<CollisionShape>();

	shape->SetBox(Vector3::ONE);
}

void JN_ObjectFactory::CreateMushroom(Vector3 pos, Quaternion rotation, float scale)
{
	Node* node = CreateObject("Mushroom", "Models/Mushroom.mdl", "Materials/Mushroom.xml", pos, rotation, scale, 0.0f);

	CollisionShape* shape = node->CreateComponent<CollisionShape>();

	StaticModel* object = node->GetComponent<StaticModel>();

	shape->SetTriangleMesh(object->GetModel(), 0);
}

void JN_ObjectFactory::CreateBox(Vector3 pos, Quaternion rotation, float scale)
{
	Node* node = CreateObject("Box", "Models/Box.mdl", "Materials/Stone.xml", pos, rotation, scale, scale * 2.0f);

	CollisionShape* shape = node->CreateComponent<CollisionShape>();

	shape->SetBox(Vector3::ONE);
}

void JN_ObjectFactory::CreateText(Text*& txt, HorizontalAlignment allignment, String name, String font, int size, Vector2 pos, bool isVisible)
{
	txt = root->CreateChild<Text>(name);

	txt->SetFont(cache->GetResource<Font>(font), size);
	txt->SetTextAlignment(HA_LEFT);
	txt->SetHorizontalAlignment(allignment);
	txt->SetVerticalAlignment(VA_TOP);
	txt->SetPosition(pos.x_, pos.y_);
	txt->SetVisible(isVisible);
}

Node* JN_ObjectFactory::CreateObject(String name, String model, String mat, 
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
