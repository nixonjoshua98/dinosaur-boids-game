#pragma once

namespace Urho3D
{
	class Text;
	class UI;
	class UIElement;
	class ResourceCache;
}

using namespace Urho3D;

class InterfaceFactory
{
public:
	InterfaceFactory(UI*, ResourceCache*);

	void CreateText(Text*&, HorizontalAlignment, String, String, int, Vector2, String = "");

private:
	UIElement* root;
	ResourceCache* cache;
};