#pragma once

#include "Character.h"

#include "StraightProjectile.h"
#include "ScatterGun.h"

class Player
{
private:
	bool assignedCharacter = false;
	bool usingCharacterControls = false;

	WeakPtr<Node> characterNode;

	Controls controls;

	StraightProjectile* straightProjectile;
	ScatterGun* scatterGun;

	int weaponID = 0;

public:
	Player();
	Player(Node* _characterNode, bool _usingCharacterControls = false);

	void DestroyCharacter() { characterNode->Remove(); }
	void UpdateMissile(float delta);
	void Shoot(Vector3 origin, Vector3 dir);
	void SetScore(int score);

	std::vector<Vector3> GetProjectilePositions();

	void ToggleWeapon();
	void DestroyProjectiles();

	int GetScore()				{ return GetCharacter()->score; }
	bool AssignedCharacter()	{ return assignedCharacter; }

	String GetWeaponName();

	WeakPtr<Node> GetCharacterNode()	{ return characterNode; }
	Vector3 GetCharacterPosition()		{ return GetCharacter()->GetPosition(); }
	Character* GetCharacter()			{ return characterNode->GetComponent<Character>(); }
	Controls& GetControls()				{ return usingCharacterControls ? GetCharacter()->controls_ : controls; }
};