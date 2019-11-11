
#include "PauseMenu.h"

PauseMenu::PauseMenu(UI* _ui, ResourceCache* _cache) : InterfaceBaseClass(_ui, _cache)
{
	
}

void PauseMenu::Create()
{
	if (isCreated) return;

	window		= CreateWindow("PauseWindow");
	continueBtn = CreateButton(window, "Continue");
	quitBtn		= CreateButton(window, "Quit");

	root->AddChild(window);

	isCreated = true;
}