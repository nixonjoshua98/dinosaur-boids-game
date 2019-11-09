
#include "InterfaceBaseClass.h"

class MainMenu : public InterfaceBaseClass
{
public:

	MainMenu(UI*, ResourceCache*);

	void Create();

	Button* offlinePlayBtn;
	Button* hostOnlineBtn;
	Button* joinOnlineBtn;
	Button* quitBtn;
};
