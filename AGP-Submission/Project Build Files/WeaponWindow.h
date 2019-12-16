#include "InterfaceBaseClass.h"

class WeaponWindow : public InterfaceBaseClass
{

public:
	WeaponWindow(UI*, ResourceCache*);

	void Create();

	void SetText(String);

private:
	Text* weaponTxt;
};