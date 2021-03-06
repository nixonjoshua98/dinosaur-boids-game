#include "InterfaceBaseClass.h"

class ChatWindow : public InterfaceBaseClass
{

public:
	ChatWindow(UI*, ResourceCache*);

	void Create();

	void AddMessage(String);

	void Clear();

	Button* sendBtn;

	LineEdit* msgBox;

private:
	ListView* chatView;
};