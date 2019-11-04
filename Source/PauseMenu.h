
#include "InterfaceBaseClass.h"

namespace Urho3D
{
	class Text;
	class ResourceCache;
}

class PauseMenu : public InterfaceBaseClass
{
public:

	PauseMenu(UI*, ResourceCache*);

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

	bool isShowing = false;

};