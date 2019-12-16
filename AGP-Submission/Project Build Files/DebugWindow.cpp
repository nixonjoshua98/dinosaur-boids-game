
#include "DebugWindow.h"

DebugWindow::DebugWindow(UI* _ui, ResourceCache* _cache) : InterfaceBaseClass(_ui, _cache)
{
	Create();
}

void DebugWindow::Create()
{
	if (isCreated) return;

	window		= CreateWindow("DebugWindow", LM_HORIZONTAL, { 300, -350 });
	fpsText		= CreateText(window, "FPS_TEXT");
	boidText	= CreateText(window, "BOID_TEXT");
	threadText	= CreateText(window, "THREAD_TEXT");

	root->AddChild(window);

	isCreated = true;
}


void DebugWindow::SetText(int fps, int boids, int threads)
{
	fpsText->SetText("FPS: " + String(fps));
	boidText->SetText("Boids: " + String(boids));
	threadText->SetText("Threads: 1 + (" + String(threads) + ")");
}
