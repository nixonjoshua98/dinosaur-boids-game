#include "GameoverWindow.h"

GameoverWindow::GameoverWindow(UI* _ui, ResourceCache* _cache) : InterfaceBaseClass(_ui, _cache)
{
	Create();
}

void GameoverWindow::Create()
{
	if (isCreated) return;

	window = CreateWindow("GameoverWindow", { 0, 0 });

	menuBtn = CreateButton(window, "Menu");

	root->AddChild(window);

	isCreated = true;
}
