
#pragma once

#include "Projectile.h"

#include <vector>

class StraightProjectile
{
public:
	void Initialise(ResourceCache* _cache, Scene* _scene);

	void Update(float delta);

	void Shoot(Vector3 origin, Vector3 dir);

	void Disable();

	std::vector<Vector3> GetPositions();

	String GetName();

private:
	const int NUM_PROJECTILES = 5;

	float cooldown;

	std::vector<Projectile> projectiles;
};