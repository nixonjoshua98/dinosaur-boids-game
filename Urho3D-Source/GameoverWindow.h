#include "InterfaceBaseClass.h"

class GameoverWindow : public InterfaceBaseClass
{

public:
	GameoverWindow(UI*, ResourceCache*);

	void Create();

	Button* menuBtn;
};