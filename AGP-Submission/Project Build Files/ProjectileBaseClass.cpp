#include "ProjectileBaseClass.h"

#include <iostream>

ProjectileBaseClass::~ProjectileBaseClass()
{
	std::cout << "Dead\n";

	for (int i = 0; i < numNodes; i++)
		delete nodes[i];

	delete[] nodes;
}

void ProjectileBaseClass::Shoot(Vector3 origin, Vector3 dir)
{
	if (readyToShoot)
	{
		origin += dir * 3.0f;

		remainingLifetime = projectileLifetime;

		for (int i = 0; i < numNodes; i++)
		{
			Node* n = nodes[i];

			RigidBody* rb = n->GetComponent<RigidBody>();

			rb->SetPosition(origin);
			rb->SetLinearVelocity(dir.Normalized() * projectileSpeed);

			n->SetEnabled(true);
		}

		readyToShoot = false;
	}
}

void ProjectileBaseClass::Disable()
{
	readyToShoot = true;

	for (int i = 0; i < numNodes; i++)
	{
		nodes[i]->SetEnabled(false);
	}
}

bool ProjectileBaseClass::IsEnabled()
{
	for (int i = 0; i < numNodes; i++)
	{
		if (nodes[i]->IsEnabled())
		{
			return true;
		}
	}

	return false;
}

std::vector<Vector3> ProjectileBaseClass::GetPositions()
{
	std::vector<Vector3> pos;

	for (int i = 0; i < numNodes; i++)
	{
		if (nodes[i]->IsEnabled())
		{
			pos.push_back(nodes[i]->GetPosition());
		}
	}

	return pos;
}
