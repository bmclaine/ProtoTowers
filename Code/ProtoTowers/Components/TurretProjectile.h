// Author : Jack Gillette
//
// Base Projectile Class for Turrets

#pragma once
#include "Components\IComponent.h"

struct DamageInfo;

class TurretProjectile : public IComponent
{
protected:
	// === Variables
	DamageInfo* m_pDamageInfo;
	float m_fSpeed;
	float m_fRange;
	float m_fTravelledDist;
	bool m_bDestroyed;

	// === Protected Interface === //
	virtual IComponent& operator=(IComponent& _rhs) override;
	TurretProjectile& operator=(const TurretProjectile& _rhs);
	virtual IComponent* CreateCopy(GameObject* const gameObject, Transform* const transform) override;
	unsigned int GetEnemyLayer();
	// =========================== //

public:
	// === TEMP
	Vector3 m_f3Direction;

	// === Constructor / Destructor === //
	TurretProjectile(GameObject* _gameObject, Transform* _transform);
	virtual ~TurretProjectile();
	// ================================ //

	// === Interface === //
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap) override;

	virtual void OnEnable() override;
	virtual void OnDisable() override;
	virtual void Update() override;
	// ================= //

	// === Collision Events === //
	virtual void OnTriggerEnter(Collider* _collider) override;
	// ======================== //

	// === Accessors === //

	// === GetDamageInfo
	//  IN : void
	//  OUT : DamageInfo* - Pointer to the DamageInfo structure
	//
	//  Returns a pointer to the DamageInfo Structure this Projectile is using.
	DamageInfo* GetDamageInfo() const;
	// === GetDamage
	//  IN : void
	//  OUT : float - The Damage value
	//
	//  Returns the Damage value from the DamageInfo structure this projectile is using.
	float GetDamage() const;
	// === GetSpeed
	//  IN : void
	//  OUT : float - The Speed value of the projectile
	//
	//  Returns the Speed value of the projectile, which is in Units per Second.
	float GetSpeed() const;
	// === GetRange
	//  IN : void
	//  OUT : The Range value of the projectile
	//
	//  Returns the Range value of the projectile, which is how far the projectile can travel before destroying itself.
	float GetRange() const;
	// ================= //

	// === Mutators === //

	// === SetDamage
	//  IN : DamageInfo - The DamageInfo structure to copy from
	//  OUT : void
	//
	//  Sets the values of this projectile's DamageInfo to match the one that is passed in.
	void SetDamage(DamageInfo _damInfo);
	// === SetDamage
	//  IN : float - Damage value
	//       float - (Optional) Minion multiplier value
	//       float - (Optional) Player multiplier value
	//       float - (Optional) Tower multipler value
	//  OUT : void
	//
	//  Sets the individual parts of this projectiles DamageInfo structure.
	void SetDamage(float _damage, float _minionMulti = 1.0f, float _playerMulti = 1.0f, float _towerMulti = 1.0f);
	// === SetSpeed
	//  IN : float - Speed value
	//  OUT : void
	// 
	//  Sets the projectile's speed value, using a Unit's per Second method.
	void SetSpeed(float _speed);
	// === SetRange
	//  IN : float - The Range value
	//  OUT : void
	//
	//  Sets how far the projectile can travel, in Units, before it destroys itself.
	void SetRange(float _range);
	// ================ //
};

