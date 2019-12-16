#include "StraightProjectile.h"

void StraightProjectile::Initialise(ResourceCache* _cache, Scene* _scene)
{
	projectiles = std::vector<Projectile>(NUM_PROJECTILES);

	for (int i = 0; i < NUM_PROJECTILES; ++i)
	{
		projectiles[i].Initialise(_cache, _scene);
	}
}

void StraightProjectile::Update(float delta)
{
	cooldown -= delta;

	for (int i = 0; i < NUM_PROJECTILES; ++i)
	{
		projectiles[i].Update(delta);
	}
}

void StraightProjectile::Shoot(Vector3 origin, Vector3 dir)
{	
	if (cooldown > 0.0f)
		return;
	
	for (int i = 0; i < NUM_PROJECTILES; ++i)
	{
		if (!projectiles[i].IsEnabled())
		{
			projectiles[i].Shoot(origin, dir);

			cooldown = 0.15f;

			break;
		}
	}
}

void StraightProjectile::Disable()
{
	for (int i = 0; i < NUM_PROJECTILES; ++i)
	{
		projectiles[i].Disable();
	}
}

std::vector<Vector3> StraightProjectile::GetPositions()
{
	std::vector<Vector3> positions;

	for (int i = 0; i < NUM_PROJECTILES; ++i)
	{
		auto temp = projectiles[i].GetPositions();

		for (Vector3 v : temp)
		{
			positions.push_back(v);
		}
	}

	return positions;
}

String StraightProjectile::GetName()
{
	return projectiles[0].GetName();
}


