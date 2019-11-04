
#include "GameMenu.h"

GameMenu::GameMenu(UI* _ui, ResourceCache* _cache) : InterfaceBaseClass(_ui, _cache)
{
	
}

void GameMenu::Create()
{
	const String controls = 
		"WASD    - Move\n"
		"F1      - Toggle camera\n"
		"F2      - Debug HUD\n"
		"F11     - Fullscreen\n"
		"Esc     - Pause";

	// Menu
	menuWindow	= CreateWindow("PauseWindow");
	continueBtn = CreateButton(menuWindow, "Continue");
	quitBtn		= CreateButton(menuWindow, "Quit");

	// Debug Info
	debugWindow = CreateWindow("DebugWindow", LM_HORIZONTAL, { 300, -350 });
	fpsText		= CreateText(debugWindow, "FPS_TEXT");
	boidText	= CreateText(debugWindow, "BOID_TEXT");
	threadText	= CreateText(debugWindow, "THREAD_TEXT");

	// Controls
	controlsWindow	= CreateWindow("ControlsWindow", LM_HORIZONTAL, { -300, -315 });
	controlsText	= CreateText(controlsWindow, controls);

	root->AddChild(menuWindow);
	root->AddChild(debugWindow);
	root->AddChild(controlsWindow);

	menuWindow->AddChild(continueBtn);
	menuWindow->AddChild(quitBtn);

	menuWindow->SetVisible(false);
	debugWindow->SetVisible(false);
}

void GameMenu::Toggle()
{
	isShowing = !isShowing;

	menuWindow->SetVisible(isShowing);
	debugWindow->SetVisible(isShowing);
}

void GameMenu::SetText(int fps, int boids, int threads)
{
	fpsText->SetText("FPS: " + String(fps));
	boidText->SetText("Boids: " + String(boids));
	threadText->SetText("Threads: 1 + (" + String(threads) + ")");
}