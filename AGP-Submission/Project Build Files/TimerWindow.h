#include "InterfaceBaseClass.h"

class TimerWindow : public InterfaceBaseClass
{

public:
	TimerWindow(UI*, ResourceCache*);

	void Create();

	void SetText(int);

private:
	Text* timerText;
};