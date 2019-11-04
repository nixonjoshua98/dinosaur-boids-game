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

#include <Urho3D/Resource/ResourceCache.h>

using namespace Urho3D;

class InterfaceBaseClass
{
public:
	InterfaceBaseClass(UI*, ResourceCache*);

protected:
	UI* ui;
	UIElement* root;
	ResourceCache* cache;

	bool isShowing = false;

	Window* CreateWindow(String, LayoutMode = LM_VERTICAL, IntVector2 pos = IntVector2::ZERO);
	Button* CreateButton(UIElement*, String);
	Text* CreateText(UIElement*, String);
};