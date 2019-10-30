
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/Button.h>

#include "PauseMenu.h"

PauseMenu::PauseMenu(UI* _ui, ResourceCache* _cache) : InterfaceBaseClass(_cache)
{
	root = _ui->GetRoot();
}

void PauseMenu::Create()
{
	const String controls = 
		"WASD - Move\n"
		"F1   - Toggle camera\n"
		"F11  - Fullscreen\n"
		"Esc  - Pause Menu";

	controlsText	= CreateText(root, controls,	HA_LEFT	, { 5, 0 });
	FPSText			= CreateText(root, "FPS_TEXT",	HA_RIGHT, {-5, 0 });
	boidText		= CreateText(root, "BOID_TEXT", HA_RIGHT, {-150, 0 });

	continueBtn		= CreateButton(root, "Continue", { 400, 300 }, 50, 25, 32);
	quitBtn			= CreateButton(root, "Quit",	 { 400, 350 }, 50, 25, 32);
}

void PauseMenu::Toggle()
{
	isShowing = !isShowing;

	controlsText->SetVisible(isShowing);
	FPSText->SetVisible(isShowing);
	boidText->SetVisible(isShowing);

	continueBtn->SetVisible(isShowing);
	quitBtn->SetVisible(isShowing);
}

void PauseMenu::SetFPS(int fps)
{
	FPSText->SetText(String(fps));
}

void PauseMenu::SetBoidCount(int count)
{
	boidText->SetText("Boids: " + String(count));
}
