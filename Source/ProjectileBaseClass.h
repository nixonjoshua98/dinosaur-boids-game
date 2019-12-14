
#pragma once

#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/StaticModel.h>

#include <vector>

using namespace Urho3D;

class ProjectileBaseClass
{
public:
	virtual ~ProjectileBaseClass();

	void virtual Initialise(ResourceCache*, Scene*) = 0;

	void virtual Shoot(Vector3 origin, Vector3 dir);

	void virtual Update(float delta) = 0;

	void Disable();

	bool IsEnabled();

	std::vector<Vector3> GetPositions();

protected:
	Node** nodes;

	int numNodes;

	int projectileLifetime;

	float remainingLifetime;
	float projectileSpeed;

	bool readyToShoot = true;
};