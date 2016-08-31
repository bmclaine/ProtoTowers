// Author : Jack Gillette
//
// Mortar Tower that shoots an arching projectile that explodes for AOE damage once it hits the ground
#pragma once
#include "BaseTurret.h"

class MortarTower : public BaseTurret
{
private:
	// === Variables
	GameObject* m_pMortar;
	float m_fMinRange;

	// === Private Interface === //
	float PredictiveTargetting(Vector3 _targetPos);
	// ========================= //

protected:
	// === Protected Interface === //
	virtual IComponent& operator=(IComponent& _rhs) override;
	MortarTower& operator=(const MortarTower& _rhs);
	virtual IComponent* CreateCopy(GameObject* const _gameObject, Transform* const _transform) override;

	virtual void Shoot() override;
	virtual void FindTarget() override;
	virtual bool RotateTowardsTarget() override;
	virtual void RotateTowardsStartingDirection() override;
	// =========================== //

public:
	// === Constructor / Destructor === //
	MortarTower(GameObject* _gameObject, Transform* _transform);
	virtual ~MortarTower();
	// ================================ //

	// === Interface === //
	virtual void PostInit(std::map<unsigned int, Object*>& _objectMap, std::map<unsigned int, ObjectData*>& _dataMap) override;
	virtual void OnEnable() override;
	// ================= //

	// === Mutators === //
	void SetMinRange(float _minRange);
	// ================ //
};

