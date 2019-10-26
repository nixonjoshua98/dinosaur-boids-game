
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
	void SetBoidCount(int count);

	Button* GetContinueButton() { return continueBtn; }
	Button* GetQuitButton() { return quitBtn; }

	bool IsShown() { return isShowing; }

private:
	UIElement* root;

	bool isShowing = false;

	Text* controlsText;
	Text* FPSText;
	Text* boidText;

	Button* continueBtn;
	Button* quitBtn;
};