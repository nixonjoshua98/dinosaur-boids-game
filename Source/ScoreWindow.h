#include "InterfaceBaseClass.h"

class ScoreWindow : public InterfaceBaseClass
{

public:
	ScoreWindow(UI*, ResourceCache*);

	void Create();

	void SetText(int);

private:
	Text* scoreText;
};