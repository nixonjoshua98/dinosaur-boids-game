#include "WeaponWindow.h"

WeaponWindow::WeaponWindow(UI* _ui, ResourceCache* _cache) : InterfaceBaseClass(_ui, _cache)
{
	Create();
}

void WeaponWindow::Create()
{
	if (isCreated) return;

	window = CreateWindow("WeaponWindow", { 0, 350 }, 150);

	weaponTxt = CreateText(window, "");

	root->AddChild(window);

	isCreated = true;
}

void WeaponWindow::SetText(String t)
{
	weaponTxt->SetText(t);
}
