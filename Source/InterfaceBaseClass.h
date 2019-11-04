

class InterfaceBaseClass
{
public:
	InterfaceBaseClass(UI*, ResourceCache*);

protected:
	UI* ui;
	UIElement* root;
	ResourceCache* cache;

	Window* CreateWindow(String, LayoutMode = LM_VERTICAL, IntVector2 pos = IntVector2::ZERO);
	Button* CreateButton(UIElement*, String);
	Text* CreateText(UIElement*, String);
};