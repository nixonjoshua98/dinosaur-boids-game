
#include <Urho3D/Resource/ResourceCache.h>

#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/Button.h>

#include "InterfaceBaseClass.h"

InterfaceBaseClass::InterfaceBaseClass(ResourceCache* _cache)
{
	cache = _cache;
}

Text* InterfaceBaseClass::CreateText(UIElement* parent, String str, HorizontalAlignment ha, Vector2 pos, bool isVisible)
{
	Font* font = cache->GetResource<Font>("Fonts/Anonymous Pro.ttf");
	Text* txt = parent->CreateChild<Text>("Text");

	txt->SetFont(font, 16);

	txt->SetTextAlignment(HA_LEFT);
	txt->SetAlignment(ha, VA_TOP);

	txt->SetPosition(pos.x_, pos.y_);

	txt->SetVisible(isVisible);

	txt->SetText(str);

	txt->SetStyleAuto();

	return txt;
}

Button* InterfaceBaseClass::CreateButton(UIElement* parent, String str, IntVector2 pos, int w, int h, int size, bool isVisible)
{
	Font* font = cache->GetResource<Font>("Fonts/Anonymous Pro.ttf");

	Button* btn = parent->CreateChild<Button>("Button");
	Text* txt	= btn->CreateChild<Text>("Text");

	txt->SetFont(font, size);
	txt->SetAlignment(HA_CENTER, VA_CENTER);
	txt->SetText(str);

	btn->SetPosition(pos);
	btn->SetVisible(isVisible);

	btn->SetFixedWidth(w);
	btn->SetFixedHeight(h);

	btn->SetStyleAuto();

	return btn;
}
