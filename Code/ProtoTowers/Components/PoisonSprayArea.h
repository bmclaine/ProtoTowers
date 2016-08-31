// Author : Jack Gillette
//
// Represents the area of effect for the Poison Spray Turret, infecting any entities that collide with it
#pragma once
#include "Components\IComponent.h"

#include <list>

// === Forward Declarations
class Entity;
class PoisonEffect;
class SpriteRenderer;
enum Team;

class PoisonSprayArea : public IComponent
{
private:
	// === Structures
	struct InfectedEntity {
		Entity* pEntity;
		PoisonEffect* pEffect;

		InfectedEntity(Entity* _entity, PoisonEffect* _effect) {
			pEntity = _entity;
			pEffect = _effect;
		}
	};

	// === Variables
	std::list<InfectedEntity> m_lInfectedEntities;
	SpriteRenderer* m_pGroundEffect;
	float m_fDamage;
	float m_fElapsedTime;
	Team m_eEnemyTeam;

	// === Private Interface === //
	bool AlreadyTrackingEntity(Entity* _entity);
	// ========================= //
	
protected:
	// === Protected Interface === //
	virtual IComponent& operator=(IComponent& _rhs) override;
	PoisonSprayArea& operator=(const PoisonSprayArea& _rhs);
	virtual IComponent* CreateCopy(GameObject* const _gameObject, Transform* const _transform) override;
	// =========================== //

public:
	// === Constructor / Destructor === //
	PoisonSprayArea(GameObject* _gameObject, Transform* _transform);
	virtual ~PoisonSprayArea();
	// ================================ //

	// === Interface === //
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap) override;
	virtual void OnEnable() override;
	virtual void OnDisable() override;
	virtual void Update() override;
	// ================= //

	// === CollisionEvents === //
	virtual void OnTriggerEnter(Collider* _collider) override;
	virtual void OnTriggerExit(Collider* _collider) override;
	// ======================= //

	// === Mutators === //
	void SetDamage(float _damage);
	void SetEnemyTeam(Team _enemyTeam);
	// ================ //
};

