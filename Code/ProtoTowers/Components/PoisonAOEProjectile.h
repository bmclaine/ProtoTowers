// Author : Jack Gillette
//
// Poison Projectile that archs towards it's target, then on impact creates a Poison Area that lasts for a bit, infecting any enemies walking through it
#pragma once
#include "TurretProjectile.h"

class PoisonAOEProjectile : public TurretProjectile
{
private:
	// === Variables
	Vector3 m_v3StartingPosition;

	// === Private Interface === //
	Team GetEnemyTeam() const;
	// ========================= //

protected:
	// === Protected Interface === //
	virtual IComponent& operator=(IComponent& _rhs) override;
	PoisonAOEProjectile& operator=(const PoisonAOEProjectile& _rhs);
	virtual IComponent* CreateCopy(GameObject* const gameObject, Transform* const transform) override;
	// =========================== //

public:
	// === Constructor / Destructor === //
	PoisonAOEProjectile(GameObject* _gameObject, Transform* _transform);
	virtual ~PoisonAOEProjectile();
	// ================================ //

	// === Interface === //
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap) override;
	virtual void OnEnable() override;
	virtual void Update() override;
	// ================= //

	// === Collision Events === //
	virtual void OnTriggerEnter(Collider* _collider) override;
	// ======================== //
};

