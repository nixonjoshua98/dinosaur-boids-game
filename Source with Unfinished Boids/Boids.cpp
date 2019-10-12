
#include "Boids.h"

float Boid::Range_FAttract = 30.0f;
float Boid::Range_FRepel = 20.0f;
float Boid::Range_FAlign = 5.0f;
float Boid::FAttract_Vmax = 5.0f;
float Boid::FAttract_Factor = 4.0f;
float Boid::FRepel_Factor = 2.0f;
float Boid::FAlign_Factor = 2.0f;

void Boid::Initialise(ResourceCache* cache, Scene* scene)
{
	pNode = scene->CreateChild("Boid");
	pStaticModel = pNode->CreateComponent<StaticModel>();
	pRigidBody = pNode->CreateComponent<RigidBody>();
	PCollisionShape = pNode->CreateComponent<CollisionShape>();

	pStaticModel->SetModel(cache->GetResource<Model>("Models/Cone.mdl"));

	pRigidBody->SetMass(1.0f);
	pRigidBody->SetUseGravity(false);
	pRigidBody->SetPosition({Random(180.0f) - 90.0f, 30.0f, Random(180.0f) - 90.0f });
	pRigidBody->SetLinearVelocity({Random(-20.0f, 20.0f), 0.0f, Random(-20.0f, 20.0f)});
}

void Boid::ComputeForce(Boid* boidList)
{
	Vector3 CoM; //centre of mass, accumulated total
	
	int n = 0; //count number of neigbours
	
	//set the force member variable to zero
	force = Vector3(0, 0, 0);

	//Search Neighbourhood
	for (int i = 0; i < 100; i++)
	{
		//the current boid?
		if (this == &boidList[i])
			continue;

		//sep = vector position of this boid from current oid
		Vector3 sep = pRigidBody->GetPosition() - boidList[i].pRigidBody->GetPosition();
		float d = sep.Length(); //distance of boid
		if (d < Range_FAttract)
		{
			//with range, so is a neighbour
			CoM += boidList[i].pRigidBody->GetPosition();
			n++;
		}
	}

	//Attractive force component
	if (n > 0)
	{
		CoM /= n;
		Vector3 dir = (CoM - pRigidBody->GetPosition()).Normalized();
		Vector3 vDesired = dir * FAttract_Vmax;
		force += (vDesired - pRigidBody->GetLinearVelocity()) * FAttract_Factor;
	}
}

void Boid::Update(float tm)
{
	pRigidBody->ApplyForce(force);

	Vector3 vel = pRigidBody->GetLinearVelocity();
	float d = vel.Length();

	if (d < 10.0f)
	{
		d = 10.0f;
		pRigidBody->SetLinearVelocity(vel.Normalized() * d);
	}

	else if (d > 1.0f)
	{
		d = 50.0f;
		pRigidBody->SetLinearVelocity(vel.Normalized() * d);
	}

	Vector3 vn = vel.Normalized();
	Vector3 cp = -vn.CrossProduct(Vector3(0.0f, 1.0f, 0.0f));
	float dp = cp.DotProduct(vn);
	pRigidBody->SetRotation(Quaternion(Acos(dp), cp));
	Vector3 p = pRigidBody->GetPosition();

	if (p.y_ < 10.0f)
	{
		p.y_ = 10.0f;
		pRigidBody->SetPosition(p);
	}

	else if (p.y_ > 50.0f)
	{
		p.y_ = 50.0f;
		pRigidBody->SetPosition(p);
	}
}
