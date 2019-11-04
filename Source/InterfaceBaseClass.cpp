
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/Window.h>
#include <Urho3D/UI/LineEdit.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/CheckBox.h>

#include <Urho3D/Resource/ResourceCache.h>

using namespace Urho3D;

#include "InterfaceBaseClass.h"

InterfaceBaseClass::InterfaceBaseClass(UI* _ui, ResourceCache* _cache)
{
	root = _ui->GetRoot();
	cache = _cache;
	ui = _ui;
}

Window* InterfaceBaseClass::CreateWindow(String name, LayoutMode layMode, IntVector2 pos)
{
	Context* context	= root->GetContext();
	Window* menuWindow		= new Window(context);
	
	menuWindow->SetMinWidth(384);
	menuWindow->SetLayout(layMode, 6, IntRect(6, 6, 6, 6));
	menuWindow->SetAlignment(HA_CENTER, VA_CENTER);
	menuWindow->SetName(name);
	menuWindow->SetStyleAuto();
	menuWindow->SetPosition(pos);

	return menuWindow;
}

Button* InterfaceBaseClass::CreateButton(UIElement* parent, String label)
{
	Button* btn = parent->CreateChild<Button>();
	Text* txt	= CreateText(btn, label);

	btn->SetMinHeight(24);
	btn->SetStyleAuto();

	return btn;
}

Text* InterfaceBaseClass::CreateText(UIElement* parent, String label)
{

	Font* font = cache->GetResource<Font>("Fonts/Anonymous Pro.ttf");
	Text* text = parent->CreateChild<Text>();

	text->SetFont(font, 12);
	text->SetAlignment(HA_CENTER, VA_CENTER);
	text->SetText(label);
	text->SetStyleAuto();

	return text;

}
