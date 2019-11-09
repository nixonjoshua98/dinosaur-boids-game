
#include "InterfaceBaseClass.h"

class GameMenu : public InterfaceBaseClass
{

public:

	GameMenu(UI*, ResourceCache*);

	void Create();

	Button* continueBtn;
	Button* quitBtn;
};