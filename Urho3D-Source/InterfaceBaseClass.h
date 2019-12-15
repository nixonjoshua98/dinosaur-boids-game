#pragma once

#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/Window.h>
#include <Urho3D/UI/LineEdit.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/CheckBox.h>
#include <Urho3D/UI/ListView.h>

#include <Urho3D/Resource/ResourceCache.h>

using namespace Urho3D;

class InterfaceBaseClass
{
public:
	InterfaceBaseClass(UI*, ResourceCache*);

	void Toggle();
	void Hide();
	void Show();

	bool IsShown() { return isShowing; }

protected:
	UI* ui;
	UIElement* root;
	ResourceCache* cache;

	SharedPtr<Window> window;

	bool isShowing = false;
	bool isCreated = false;

	Window* CreateWindow(String);
	Window* CreateWindow(String, LayoutMode, IntVector2);
	Window* CreateWindow(String, IntVector2);
	Window* CreateWindow(String, IntVector2, int);

	Button* CreateButton(UIElement*, String);
	Text* CreateText(UIElement*, String);
	LineEdit* CreateLineEdit(UIElement*, String);

	ListView* CreateListView(UIElement*, int);

	Window* CreateWindow(String name, LayoutMode layout, IntVector2 pos, int width);
};