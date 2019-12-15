#include "InterfaceBaseClass.h"

class ControlsWindow : public InterfaceBaseClass
{

public:
	ControlsWindow(UI*, ResourceCache*);

	void Create();

private:
	Text* controlsText;
};