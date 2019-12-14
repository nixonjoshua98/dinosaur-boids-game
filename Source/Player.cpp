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
	characterNode = _characterNode;

	assignedCharacter = true;
	usingCharacterControls = _usingCharacterControls;

	straightProjectile.Initialise(_characterNode->GetContext()->GetSubsystem<ResourceCache>(), _characterNode->GetScene());
}

void Player::UpdateMissile(float delta)
{
	straightProjectile.Update(delta);
}

void Player::Shoot(Vector3 origin, Vector3 dir)
{
	straightProjectile.Shoot(origin, dir);
}

void Player::SetScore(int score)
{
	GetCharacter()->score = score;
}

std::vector<Vector3> Player::GetProjectilePositions()
{
	return straightProjectile.GetPositions();
}
