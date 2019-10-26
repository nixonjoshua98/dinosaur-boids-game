#pragma once


namespace Urho3D
{
	class Text;
	class UIElement;
	class ResourceCache;
	class Button;
}

using namespace Urho3D;

class InterfaceBaseClass
{
public:
	InterfaceBaseClass(ResourceCache*);

	Text* CreateText(UIElement*, String, HorizontalAlignment, Vector2, bool = false);
	Button* CreateButton(UIElement*, String, IntVector2, int, int, int, bool = false);

private:
	ResourceCache* cache;
};