// Author : Jack Gillette
//
// Projectile class that follows an arch path to it's target and blows up once it hits the ground, dealing AOE damage to any nearby enemies.
#pragma once
#include "Components\IComponent.h"

#include "MathLib.h"

class MortarProjectile : public IComponent
{
private:
	// === Variables
	Vector3 m_v3StartingPosition;
	Vector3 m_v3Direction;
	float m_fSpeed;
	float m_fTargetDistance;

protected:
	// === Shared Variables
	float m_fDamage;
	float m_fRadius;

	// === Protected Interface === //
	virtual IComponent& operator=(IComponent& _rhs) override;
	MortarProjectile& operator=(const MortarProjectile& _rhs);
	virtual IComponent* CreateCopy(GameObject* const gameObject, Transform* const transform) override;

	unsigned int GetEnemyLayer();
	virtual void Explode();
	// =========================== //

public:
	// === Constructor / Destructor === //
	MortarProjectile(GameObject* _gameObject, Transform* _transform);
	virtual ~MortarProjectile();
	// ================================ //

	// === Interface === //
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap) override;

	virtual void OnEnable() override;
	virtual void Update() override;
	// ================= //

	// === Accessors === //
	float GetSpeed() const;
	float GetTargetDistance() const;
	float GetDamage() const;
	// ================= //

	// === Mutators === //
	void SetDirection(Vector3 _direction);
	void SetSpeed(float _speed);
	void SetTargetDistance(float _distance);
	void SetDamage(float _damage);
	void SetRadius(float _radius);
	// ================ //
};

