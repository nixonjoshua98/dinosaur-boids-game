
#include "InterfaceBaseClass.h"

class GameMenu : public InterfaceBaseClass
{

public:

	GameMenu(UI*, ResourceCache*);

	void Create();
	void Toggle();

	void SetText(int, int, int);

	bool IsShown() { return isShowing; }

	Button* GetQuitBtn() { return quitBtn; };
	Button* GetContinueBtn() { return continueBtn; };

private:

	// Widgets
	SharedPtr<Window> menuWindow;
	SharedPtr<Window> debugWindow;
	SharedPtr<Window> controlsWindow;

	Button* continueBtn;
	Button* quitBtn;

	Text* fpsText;
	Text* boidText;
	Text* threadText;
	Text* controlsText;

};