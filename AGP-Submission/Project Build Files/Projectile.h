
#pragma once

#include "ProjectileBaseClass.h"

class Projectile : public ProjectileBaseClass
{
public:
	void Initialise(ResourceCache* _cache, Scene* _scene) override;

	void Update(float delta) override;

	void Shoot(Vector3 origin, Vector3 dir) override;

private:
	float yShotAt;
};