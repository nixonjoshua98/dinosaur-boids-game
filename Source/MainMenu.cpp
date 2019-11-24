
#include "MainMenu.h"

MainMenu::MainMenu(UI* _ui, ResourceCache* _cache) : InterfaceBaseClass(_ui, _cache)
{
	Create();
}

void MainMenu::Create()
{
	if (isCreated) return;

	window = CreateWindow("MainWindow");

	offlinePlayBtn	= CreateButton(window, "Play Offline");
	ipAddress		= CreateLineEdit(window, "localhost");
	hostOnlineBtn	= CreateButton(window, "Host Online Game");
	joinOnlineBtn	= CreateButton(window, "Join Online Game");
	quitBtn			= CreateButton(window, "Quit");

	root->AddChild(window);

	isCreated = true;
}