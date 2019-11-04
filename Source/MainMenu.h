
#include "InterfaceBaseClass.h"

class MainMenu : public InterfaceBaseClass
{
public:

	MainMenu(UI*, ResourceCache*);

	void Create();
	void Toggle();

	Button* GetPlayBtn() { return offlinePlayBtn; };
	Button* GetHostBtn() { return hostOnlineBtn; };
	Button* GetJoinBtn() { return joinOnlineBtn; };
	Button* GetQuitBtn() { return quitBtn;  };

	bool IsShown() { return isShowing; }

private:

	// Widgets
	SharedPtr<Window> window;

	Button* offlinePlayBtn;
	Button* hostOnlineBtn;
	Button* joinOnlineBtn;
	Button* quitBtn;
};