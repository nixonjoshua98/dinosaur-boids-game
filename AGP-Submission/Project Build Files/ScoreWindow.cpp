#include "ScoreWindow.h"

ScoreWindow::ScoreWindow(UI* _ui, ResourceCache* _cache) : InterfaceBaseClass(_ui, _cache)
{
	Create();
}

void ScoreWindow::Create()
{
	if (isCreated) return;

	window		= CreateWindow("ScoreWindow", { 400, -350 }, 100);
	scoreText	= CreateText(window, "Score: 0");

	root->AddChild(window);

	isCreated = true;
}

void ScoreWindow::SetText(int score)
{
	scoreText->SetText("Score: " + String(score));
}
