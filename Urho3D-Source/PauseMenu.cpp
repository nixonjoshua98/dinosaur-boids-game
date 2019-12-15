
#include "PauseMenu.h"

PauseMenu::PauseMenu(UI* _ui, ResourceCache* _cache) : InterfaceBaseClass(_ui, _cache)
{
	Create();
}

void PauseMenu::Create()
{
	if (isCreated) return;

	window		= CreateWindow("PauseWindow");
	continueBtn	= CreateButton(window, "Continue");

	disconnectBtn	= CreateButton(window, "Main Menu");

	quitBtn	= CreateButton(window, "Quit to Desktop");

	root->AddChild(window);

	isCreated = true;
}