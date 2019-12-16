#include "Projectile.h"

#include <iostream>

void Projectile::Initialise(ResourceCache* _cache, Scene* _scene)
{
	projectileLifetime	= 1.f;
	projectileSpeed		= 75.0f;

	name = "Semi-Auto Sphere";

	numNodes = 1;

	nodes = new Node * [numNodes];

	for (int i = 0; i < numNodes; i++)
	{
		nodes[i] = _scene->CreateChild("Projectile", REPLICATED);

		StaticModel* staticModel = nodes[i]->CreateComponent<StaticModel>(REPLICATED);

		RigidBody* rb = nodes[i]->CreateComponent<RigidBody>(LOCAL);

		staticModel->SetModel(_cache->GetResource<Model>("Models/Sphere.mdl"));
		staticModel->SetMaterial(_cache->GetResource<Material>("Materials/Stone.xml"));

		nodes[i]->SetScale(0.5f);

		rb->SetMass(1.0f);
		rb->SetUseGravity(false);
		rb->SetPosition({ 0.0f, 0.0f, 0.0f });

		nodes[i]->SetEnabled(false);
	}
}

void Projectile::Update(float delta)
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
			for (int i = 0; i < numNodes; i++)
			{
				RigidBody* rb = nodes[i]->GetComponent<RigidBody>();

				Vector3 linearV = rb->GetLinearVelocity();
				Vector3 pos = rb->GetPosition();

				linearV.y_ = yShotAt;
				pos.y_ = yShotAt;

				rb->SetLinearVelocity(linearV);
				rb->SetPosition(pos);
			}
		}
	}
}

void Projectile::Shoot(Vector3 origin, Vector3 dir)
{
	yShotAt = origin.y_ + 0.5f;

	ProjectileBaseClass::Shoot(origin, dir);
}
