#pragma once

#include "Character.h"

#include "StraightProjectile.h"

class Player
{
private:
	bool assignedCharacter = false;
	bool usingCharacterControls = false;

	WeakPtr<Node> characterNode;

	Controls controls;
	StraightProjectile straightProjectile;

public:
	Player();
	Player(Node* _characterNode, bool _usingCharacterControls = false);

	void DestroyCharacter() { characterNode->Remove(); }
	void UpdateMissile(float delta);
	void Shoot(Vector3 origin, Vector3 dir);
	void SetScore(int score);

	std::vector<Vector3> GetProjectilePositions();

	int GetScore()				{ return GetCharacter()->score; }
	bool AssignedCharacter()	{ return assignedCharacter; }

	WeakPtr<Node> GetCharacterNode()	{ return characterNode; }
	Vector3 GetCharacterPosition()		{ return GetCharacter()->GetPosition(); }
	Character* GetCharacter()			{ return characterNode->GetComponent<Character>(); }
	Controls& GetControls()				{ return usingCharacterControls ? GetCharacter()->controls_ : controls; }
};