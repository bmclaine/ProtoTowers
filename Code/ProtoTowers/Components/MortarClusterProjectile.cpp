#include "stdafx.h"
#include "MortarClusterProjectile.h"

// === Defines
#define NUMBER_CLUSTER_BOMBS 3
#define BOMB_MIN_RANGE 1.0f
#define BOMB_MAX_RANGE 6.0f

// ===== Constructor / Destructor ===== //
MortarClusterProjectile::MortarClusterProjectile(GameObject* _gameObject, Transform* _transform) : MortarProjectile(_gameObject, _transform)
{

}

MortarClusterProjectile::~MortarClusterProjectile()
{

}
// ==================================== //

// ===== Protected Interface ===== //
IComponent& MortarClusterProjectile::operator=(IComponent& _rhs)
{
	MortarClusterProjectile* rhsMCP = dynamic_cast<MortarClusterProjectile*>(&_rhs);
	if (rhsMCP == nullptr) {
		return *this;
	}

	return operator=(*rhsMCP);
}

MortarClusterProjectile& MortarClusterProjectile::operator=(const MortarClusterProjectile& _rhs)
{
	if (this != &_rhs) {
		MortarProjectile::operator=(_rhs);
	}

	return *this;
}

IComponent* MortarClusterProjectile::CreateCopy(GameObject* const gameObject, Transform* const transform)
{
	MortarClusterProjectile* newMCP = new MortarClusterProjectile(gameObject, transform);
	*newMCP = *this;

	return newMCP;
}

void MortarClusterProjectile::Explode()
{
	// === Normal Mortar Explosion
	MortarProjectile::Explode();

	// === Spawn the Cluster bombs
	int maxAngle = (int)(360 / NUMBER_CLUSTER_BOMBS);
	float yRot;
	GameObject* projectile;
	MortarProjectile* mortar;

	for (int i = 0; i < NUMBER_CLUSTER_BOMBS; ++i) {
		// === Determine the rotation
		yRot = (float)(rand() % maxAngle) + (maxAngle * i);

		// === Spawn the bomb
		projectile = Inferno::Instantiate("MortarProjectile");
		projectile->transform->SetPosition(transform->GetPosition());
		projectile->transform->Rotate(Vector3(0.0f, ToRadians(yRot), 0.0f));
		projectile->SetLayer(gameObject->GetLayer());

		// === Settings
		mortar = projectile->GetComponent<MortarProjectile>();
		mortar->SetDamage(m_fDamage * 0.5f);
		mortar->SetRadius(m_fRadius * 0.75f);
		mortar->SetDirection(projectile->transform->Forward());
		mortar->SetSpeed(GetSpeed() * 0.25f);
		mortar->SetTargetDistance((rand() / static_cast<float>(RAND_MAX)) * (BOMB_MAX_RANGE - BOMB_MIN_RANGE));
	}
}
// =============================== //