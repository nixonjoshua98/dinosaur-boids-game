
#pragma once

using namespace Urho3D;

class Character;

class _ObjectFactory
{
public:

	void SetCache(ResourceCache*);
	void SetScene(Scene*);

	void CreateMushroom(Vector3, Quaternion, float);
	void CreateBox(Vector3, Quaternion, float);

private:
	ResourceCache* cache;
	Scene* scene;

	Node* CreateObject(String, String, String, Vector3, Quaternion, float, float);
};