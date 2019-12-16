#include "TimerWindow.h"

TimerWindow::TimerWindow(UI* _ui, ResourceCache* _cache) : InterfaceBaseClass(_ui, _cache)
{
	Create();
}

void TimerWindow::Create()
{
	if (isCreated) return;

	window = CreateWindow("TimerWindow", { 0, -350 }, 25);
	timerText = CreateText(window, "");

	root->AddChild(window);

	isCreated = true;
}

void TimerWindow::SetText(int t)
{
	timerText->SetText(String(t));
}
