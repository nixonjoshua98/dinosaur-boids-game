#pragma once

#include "InterfaceBaseClass.h"

class DebugWindow : public InterfaceBaseClass
{

public:
	DebugWindow(UI*, ResourceCache*);

	void Create();

	void SetText(int, int, int);

private:
	Text* fpsText;
	Text* boidText;
	Text* threadText;
};