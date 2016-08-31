#pragma once
#include "BaseTower.h"

#include <list>

class InfernoTimer;

class BaseTurret : public BaseTower
{
private:
	// === Private Interface === //
	void DoubleShot();
	// ========================= //

	// === Events === //
	void OnStopUpdating(void* _pData);
	// ============== //

protected:
	// === Variables
	List<Entity*> m_EnemiesInRange;
	Entity* m_pTarget;
	Transform* m_pGunTransform;
	Transform* m_pProjectileSpawn;
	InfernoTimer* m_pFireTimer;
	InfernoTimer* m_pTargetResetTimer;
	std::string m_sProjectileID;
	float m_fFireRate;
	float m_fRange;
	float m_fTurnRate;
	float m_fDamage;
	bool m_bNoUpdate;

	// === Protected Interface === //
	virtual IComponent& operator=(IComponent& _rhs) override;
	BaseTurret& operator=(const BaseTurret& _rhs);
	virtual IComponent* CreateCopy(GameObject* const _gameObject, Transform* const _transform) override;

	virtual void Shoot();
	virtual void FindTarget();
	virtual bool RotateTowardsTarget();
	virtual void RotateTowardsStartingDirection();

	virtual void OnEntityRelease(Entity* _releasedEntity);
	virtual void OnNoTargets();
	// =========================== //

public:
	// === Constructor / Destructor === //
	BaseTurret(GameObject* _gameObject, Transform* _transform);
	virtual ~BaseTurret();
	// ================================ //

	// === Interface === //
	virtual void Init() override;
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap) override;
	virtual void OnEnable() override;
	virtual void OnDisable() override;
	virtual void Update() override;
	virtual void TakeDamage(DamageInfo _damage) override;
	virtual void Death() override;
	// ================= //

	// === Collision Events === //
	virtual void OnTriggerEnter(Collider* _collider) override;
	virtual void OnTriggerExit(Collider* _collider) override;
	// ======================== //

	// === Accessors === //
	Entity* GetTarget() const;
	Team GetEnemyTeam() const;
	float GetFireRate() const;
	float GetRange() const;
	float GetTurnRate() const;
	// ================= //

	// === Mutators === //
	void SetGun(Transform* _gunTransform);
	void SetProjectileSpawn(Transform* _projSpawn);
	void SetProjectile(std::string _projectileID);
	virtual void SetFireRate(float _roundsPerSecond);
	void SetRange(float _radius);
	void SetTurnRate(float _anglesPerSecond);
	void SetDamage(float _damage);
	// ================ //
};

