
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

#include "PauseMenu.h"

PauseMenu::PauseMenu(UI* _ui, ResourceCache* _cache) : InterfaceBaseClass(_ui, _cache)
{
	
}

void PauseMenu::Create()
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
	//controlsWindow->SetVisible(false);
}

void PauseMenu::Toggle()
{
	isShowing = !isShowing;

	menuWindow->SetVisible(isShowing);
	debugWindow->SetVisible(isShowing);
	//controlsWindow->SetVisible(isShowing);
}

void PauseMenu::SetText(int fps, int boids, int threads)
{
	fpsText->SetText("FPS: " + String(fps));
	boidText->SetText("Boids: " + String(boids));
	threadText->SetText("Threads: 1 + (" + String(threads) + ")");
}
