
#include "InterfaceBaseClass.h"

class PauseMenu : public InterfaceBaseClass
{

public:

	PauseMenu(UI*, ResourceCache*);

	void Create();

	Button* continueBtn;
	Button* quitBtn;

	LineEdit* ipAddress;

	Button* connectBtn;
	Button* disconnectBtn;
};