#include "ControlsWindow.h"

ControlsWindow::ControlsWindow(UI* _ui, ResourceCache* _cache) : InterfaceBaseClass(_ui, _cache)
{
	Create();
}

void ControlsWindow::Create()
{
	if (isCreated) return;

	const String controls =
		"WASD    - Move\n"
		"Space   - Jump\n"
		"LMB     - Shoot\n"
		"F       - Cycle Weapons\n"
		"F1      - Toggle First Person\n"
		"F2      - Debug HUD\n"
		"F3      - Toggle Free Camera\n"
		"F4      - Toggle Console\n"
		"F11     - Fullscreen\n"
		"Esc     - Pause";

	// Controls
	window = CreateWindow("ControlsWindow", LM_HORIZONTAL, { -300, -275 });
	controlsText = CreateText(window, controls);

	root->AddChild(window);

	isCreated = true;
}
