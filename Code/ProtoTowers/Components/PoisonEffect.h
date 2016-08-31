// Author : Jack Gillette
//
// Effact that attaches to an Entity and does damage over time, for a set amount of time.

#pragma once
#include "Components\IComponent.h"

class Entity;
class InfernoTimer;

class PoisonEffect : public IComponent
{
private:
	// === Variables
	Entity* m_pEntity;
	InfernoTimer* m_pDamageTimer;
	InfernoTimer* m_pEffectTimer;
	float m_fDamage;
	unsigned int m_uiInfectedCount;

	// === Private Interface === //
	void OnDealDamage();
	void OnRemoveEffect();
	void OnEntityRelease(Entity* _entity);
	// ========================= //

protected:
	// === Protected Interface === //
	virtual IComponent& operator=(IComponent& _rhs) override;
	PoisonEffect& operator=(const PoisonEffect& _rhs);
	virtual IComponent* CreateCopy(GameObject* const _gameObject, Transform* const _transform) override;
	// =========================== //

public:
	// === Constructor / Destructor === //
	PoisonEffect(GameObject* _gameObject, Transform* _transform);
	virtual ~PoisonEffect();
	// ================================ //

	// === Interface === //
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap) override;
	virtual void OnEnable() override;
	virtual void OnDisable() override;
	void StartEffectTimer();
	void StopEffectTimer();
	void ResetEffectTimer();
	// ================= //

	// === Mutators === //
	void SetDamage(float _damage);
	// ================ //
};

