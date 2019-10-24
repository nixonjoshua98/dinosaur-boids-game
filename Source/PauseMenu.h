
#include "InterfaceBaseClass.h"


namespace Urho3D
{
	class Text;
	class ResourceCache;
}

using namespace Urho3D;

class PauseMenu : public InterfaceBaseClass
{
public:

	PauseMenu(UI*, ResourceCache*);

	void Create();
	void Toggle();

	void SetFPS(int fps);

	Button* GetContinueButton() { return continueBtn; }
	Button* GetQuitButton() { return quitBtn; }

	bool IsShown() { return isShowing; }

private:
	UIElement* root;

	bool isShowing = false;

	Text* controlsText;
	Text* FPSText;

	Button* continueBtn;
	Button* quitBtn;
};