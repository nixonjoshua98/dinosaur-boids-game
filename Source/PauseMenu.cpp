
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
		"F5   - Save Scene\n"
		"F7   - Load Scene\n"
		"F11  - Fullscreen\n"
		"Esc  - Pause Menu";

	controlsText	= CreateText(root, controls,	HA_LEFT	, { 5, 0 });
	FPSText			= CreateText(root, "FPS_TEXT",	HA_RIGHT, {-5, 0 });

	continueBtn		= CreateButton(root, "Continue",	HA_CENTER, { 512, 384 - 25 }, 5, 5);
	quitBtn			= CreateButton(root, "Quit",		HA_CENTER, { 512, 384 + 25 }, 5, 5);
}

void PauseMenu::Toggle()
{
	isShowing = !isShowing;

	controlsText->SetVisible(isShowing);
	FPSText->SetVisible(isShowing);

	continueBtn->SetVisible(isShowing);
	quitBtn->SetVisible(isShowing);
}

void PauseMenu::SetFPS(int fps)
{
	FPSText->SetText(String(fps));
}