#pragma once

#include <Urho3D/UI/UI.h>

namespace Urho3D
{
	class Text;
	class UI;
	class UIElement;
	class ResourceCache;
	class Button;
	class Font;
}

using namespace Urho3D;

class InterfaceBaseClass
{
public:
	InterfaceBaseClass(ResourceCache*);

	Text* CreateText(UIElement*, String, HorizontalAlignment, Vector2, bool = false);
	Button* CreateButton(UIElement*, String, HorizontalAlignment, IntVector2, int, int, bool = false);

private:
	ResourceCache* cache;
};