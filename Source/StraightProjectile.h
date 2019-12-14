
#pragma once

#include "ProjectileBaseClass.h"

class StraightProjectile : public ProjectileBaseClass
{
public:
	void Initialise(ResourceCache* _cache, Scene* _scene) override;

	void Update(float delta) override;
};