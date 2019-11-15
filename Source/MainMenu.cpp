
#include "MainMenu.h"

MainMenu::MainMenu(UI* _ui, ResourceCache* _cache) : InterfaceBaseClass(_ui, _cache)
{

}

void MainMenu::Create()
{
	if (isCreated) return;

	window = CreateWindow("MainWindow");

	offlinePlayBtn = CreateButton(window, "Play Offline");
	hostOnlineBtn = CreateButton(window, "Host Online Game");
	ipAddress = CreateLineEdit(window, "localhost");
	joinOnlineBtn = CreateButton(window, "Join Online Game");
	quitBtn = CreateButton(window, "Quit");

	root->AddChild(window);

	isCreated = true;
}