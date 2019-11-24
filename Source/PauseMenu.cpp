
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

	ipAddress = CreateLineEdit(window, "localhost");

	connectBtn		= CreateButton(window, "Connect");
	disconnectBtn	= CreateButton(window, "Disconnect");
	//startSvrBtn		= CreateButton(window, "Start Server");

	quitBtn	= CreateButton(window, "Quit");

	root->AddChild(window);

	isCreated = true;
}