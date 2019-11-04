
#include "InterfaceBaseClass.h"

class MainMenu : public InterfaceBaseClass
{
public:

	MainMenu(UI*, ResourceCache*);

	void Create();
	void Toggle();

	bool IsShown() { return isShowing; }

private:

	// Widgets
	SharedPtr<Window> window;

	Button* offlinePlayBtn;
	Button* hostOnlineBtn;
	Button* joinOnlineBtn;
	Button* quitBtn;
};