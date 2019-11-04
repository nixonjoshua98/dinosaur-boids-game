
#include "MainMenu.h"

MainMenu::MainMenu(UI* _ui, ResourceCache* _cache) : InterfaceBaseClass(_ui, _cache)
{

}

void MainMenu::Create()
{
	window = CreateWindow("MainWindow");

	offlinePlayBtn	= CreateButton(window, "Play Offline");
	hostOnlineBtn	= CreateButton(window, "Host Online Game");
	joinOnlineBtn	= CreateButton(window, "Join Online Game");
	quitBtn			= CreateButton(window, "Quit");

	root->AddChild(window);

	//window->SetVisible(false);
}

void MainMenu::Toggle()
{
	isShowing = !isShowing;

	window->SetVisible(isShowing);
}
