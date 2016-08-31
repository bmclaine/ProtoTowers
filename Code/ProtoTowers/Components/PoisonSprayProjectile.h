// Author : Jack Gillette
//
// Projectile that represents the spraying of the poison turret, doesn't destroy on impact, only when it's reached it's distance.
#pragma once
#include "TurretProjectile.h"

class PoisonSprayProjectile : public TurretProjectile
{
protected:
	// === Protected Interface === //
	virtual IComponent& operator=(IComponent& _rhs) override;
	PoisonSprayProjectile& operator=(const PoisonSprayProjectile& _rhs);
	virtual IComponent* CreateCopy(GameObject* const _gameObject, Transform* const _transform) override;
	// =========================== //

public:
	// === Constructor / Destructor === //
	PoisonSprayProjectile(GameObject* _gameObject, Transform* _transform);
	virtual ~PoisonSprayProjectile();
	// ================================ //

	// === Interface === //
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap) override;
	// ================= //

	// === Collision Events === //
	virtual void OnTriggerEnter(Collider* _collider) override;
	// ======================== //
};

