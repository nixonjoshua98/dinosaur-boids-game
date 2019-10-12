
#pragma once


using namespace Urho3D;

class Character;


class JN_ObjectFactory
{
public:
	Scene* CreateScene(Context*);
	Camera* SetupCamera(SharedPtr<Node>, Context*);

	void SetCache(ResourceCache*);
	void SetScene(Scene*);
	void SetRoot(UIElement*);

	Character* CreateCharacter(String, Vector3);
	
	void CreateZone();
	void CreateLight(LightType);
	void CreateFloor();
	void CreateMushroom(Vector3, Quaternion, float);
	void CreateBox(Vector3, Quaternion, float);

	void CreateText(Text*&, HorizontalAlignment, String, String, int, Vector2, bool);

private:
	ResourceCache* cache;
	Scene* scene;
	UIElement* root;

	// Creates a generic node used to create objects such as mushroom & box
	Node* CreateObject(String, String, String, Vector3, Quaternion, float, float);
};