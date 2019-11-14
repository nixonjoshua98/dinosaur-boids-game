#include "ControlsWindow.h"

ControlsWindow::ControlsWindow(UI* _ui, ResourceCache* _cache) : InterfaceBaseClass(_ui, _cache)
{

}

void ControlsWindow::Create()
{
	if (isCreated) return;

	const String controls =
		"WASD    - Move\n"
		"F1      - Toggle First Person\n"
		"F2      - Debug HUD\n"
		"F3      - Toggle Free Camera\n"
		"F11     - Fullscreen\n"
		"Esc     - Pause";

	// Controls
	window = CreateWindow("ControlsWindow", LM_HORIZONTAL, { -300, -315 });
	controlsText = CreateText(window, controls);

	root->AddChild(window);

	isCreated = true;
}
