// Author : Jack Gillette
//
// A Debug Projectile that will cause a breakpoint to happen when it collides with anything, allowing the developer to step through the values of the collided object and see what is going on.
#pragma once
#include "Components\IComponent.h"

class DebugProjectile : public IComponent
{
private:
	// === Variables
	float m_fSpeed;

protected:
	// === Protected Interface === //
	virtual IComponent& operator=(IComponent& _rhs) override;
	DebugProjectile& operator=(const DebugProjectile& _rhs);
	virtual IComponent* CreateCopy(GameObject* const gameObject, Transform* const transform) override;
	// =========================== //

public:
	// === Constructor / Destructor === //
	DebugProjectile(GameObject* _gameObject, Transform* _transform);
	virtual ~DebugProjectile();
	// ================================ //

	// === Interface === //
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap) override;

	virtual void Update() override;
	// ================= //

	// === Collision Events === //
	virtual void OnTriggerEnter(Collider* _collider) override;
	// ======================== //
};

