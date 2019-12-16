#include <Urho3D/Resource/ResourceCache.h>

#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/Node.h>

#include <Urho3D/Core/Context.h>

#include "Player.h"


Player::Player()
{

}

Player::Player(Node* _characterNode, bool _usingCharacterControls)
{

	characterNode			= _characterNode;
	assignedCharacter		= true;
	usingCharacterControls	= _usingCharacterControls;

	straightProjectile	= new StraightProjectile();
	scatterGun			= new ScatterGun();

	straightProjectile->Initialise(_characterNode->GetContext()->GetSubsystem<ResourceCache>(), _characterNode->GetScene());
	scatterGun->Initialise(_characterNode->GetContext()->GetSubsystem<ResourceCache>(), _characterNode->GetScene());
}

void Player::UpdateMissile(float delta)
{
	straightProjectile->Update(delta);
	scatterGun->Update(delta);
}

void Player::Shoot(Vector3 origin, Vector3 dir)
{
	switch (weaponID)
	{
	case 0:
		straightProjectile->Shoot(origin, dir);
		break;

	case 1:
		scatterGun->Shoot(origin, dir);
		break;
	}
}

void Player::SetScore(int score)
{
	GetCharacter()->score = score;
}

std::vector<Vector3> Player::GetProjectilePositions()
{
	std::vector<Vector3> positions;

	auto a = straightProjectile->GetPositions();
	auto b = scatterGun->GetPositions();

	positions.reserve(a.size() + b.size());

	positions.insert(positions.end(), a.begin(), a.end());
	positions.insert(positions.end(), b.begin(), b.end());

	return positions;
}

void Player::ToggleWeapon()
{
	switch (weaponID)
	{
	case 0:
		++weaponID;
		break;

	case 1:
		weaponID = 0;
		break;
	}
}

void Player::DestroyProjectiles()
{
	straightProjectile->Disable();
	scatterGun->Disable();
}

String Player::GetWeaponName()
{
	switch (weaponID)
	{
	case 0: return straightProjectile->GetName();
	case 1: return scatterGun->GetName();
	}
}
