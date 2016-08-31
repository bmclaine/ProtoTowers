// Author : Jack Gillette
//
// Turret that uses Poison to damage entities over time.
#pragma once
#include "BaseTurret.h"

#include <list>

class BoxCollider;
class Entity;
class InfernoTimer;
class PoisonEffect;

class PoisonTurret : public BaseTurret
{
private:
	// === Variables
	InfernoTimer* m_pSecondaryProjectileTimer;
	bool m_bAOEProjectile;

protected:
	// === Protected Interface === //
	virtual IComponent& operator=(IComponent& _rhs) override;
	PoisonTurret& operator=(const PoisonTurret& _rhs);
	virtual IComponent* CreateCopy(GameObject* const _gameObject, Transform* const _transform) override;

	virtual void Shoot() override;
	// =========================== //

public:
	// === Constructor / Destructor === //
	PoisonTurret(GameObject* _gameObject, Transform* _transform);
	virtual ~PoisonTurret();
	// ================================ //

	// === Interface === //
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap) override;
	virtual void OnEnable() override;
	// ================= //
};

