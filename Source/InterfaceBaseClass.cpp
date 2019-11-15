
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

void InterfaceBaseClass::Toggle()
{
	isShowing = !isShowing;

	window->SetVisible(isShowing);
}

void InterfaceBaseClass::Hide()
{
	isShowing = true;

	Toggle();
}

void InterfaceBaseClass::Show()
{
	isShowing = false;

	Toggle();
}

Window* InterfaceBaseClass::CreateWindow(String name)
{
	return CreateWindow(name, LM_VERTICAL, { 0, 0 }, 384);
}

Window* InterfaceBaseClass::CreateWindow(String name, LayoutMode layout, IntVector2 pos)
{
	return CreateWindow(name, layout, pos, 384);
}

Window* InterfaceBaseClass::CreateWindow(String name, IntVector2 pos)
{
	return CreateWindow(name, LM_VERTICAL, pos, 384);
}

Window* InterfaceBaseClass::CreateWindow(String name, IntVector2 pos, int width)
{
	return CreateWindow(name, LM_VERTICAL, pos, width);
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

LineEdit* InterfaceBaseClass::CreateLineEdit(UIElement* parent, String txt)
{
	LineEdit* lineEdit = parent->CreateChild<LineEdit>();

	lineEdit->SetMinHeight(24);
	lineEdit->SetAlignment(HA_CENTER, VA_CENTER);
	lineEdit->SetText(txt);
	window->AddChild(lineEdit);
	lineEdit->SetStyleAuto();

	return lineEdit;
}

Window* InterfaceBaseClass::CreateWindow(String name, LayoutMode layout, IntVector2 pos, int width)
{
	Context* context = root->GetContext();
	Window* window = new Window(context);

	window->SetMinWidth(width);
	window->SetLayout(layout, 6, IntRect(6, 6, 6, 6));
	window->SetAlignment(HA_CENTER, VA_CENTER);
	window->SetName(name);
	window->SetStyleAuto();
	window->SetPosition(pos);

	return window;
}
