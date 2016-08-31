#pragma once
#include "MortarProjectile.h"

class MortarClusterProjectile : public MortarProjectile
{
protected:
	// === Protected Interface === //
	virtual IComponent& operator=(IComponent& _rhs) override;
	MortarClusterProjectile& operator=(const MortarClusterProjectile& _rhs);
	virtual IComponent* CreateCopy(GameObject* const gameObject, Transform* const transform) override;

	virtual void Explode() override;
	// =========================== //

public:
	// === Constructor / Destructor === //
	MortarClusterProjectile(GameObject* _gameObject, Transform* _transform);
	virtual ~MortarClusterProjectile();
	// ================================ //
};

