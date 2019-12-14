#include "StraightProjectile.h"

#include <iostream>

void StraightProjectile::Initialise(ResourceCache* _cache, Scene* _scene)
{
	projectileLifetime	= 1.0f;
	projectileSpeed		= 75.0f;

	numNodes = 1;

	nodes = new Node * [numNodes];

	for (int i = 0; i < numNodes; i++)
	{
		nodes[i] = _scene->CreateChild("Projectile", REPLICATED);

		StaticModel* staticModel = nodes[i]->CreateComponent<StaticModel>(REPLICATED);

		RigidBody* rb = nodes[i]->CreateComponent<RigidBody>(REPLICATED);

		staticModel->SetModel(_cache->GetResource<Model>("Models/Sphere.mdl"));
		staticModel->SetMaterial(_cache->GetResource<Material>("Materials/Stone.xml"));

		nodes[i]->SetScale(0.5f);

		rb->SetMass(1.0f);
		rb->SetUseGravity(false);
		rb->SetPosition({ 0.0f, 0.0f, 0.0f });

		nodes[i]->SetEnabled(false);
	}
}

void StraightProjectile::Update(float delta)
{
	if (IsEnabled())
	{
		remainingLifetime -= delta;

		if (remainingLifetime <= 0.0f)
		{
			Disable();
		}
		else
		{

		}
	}
}