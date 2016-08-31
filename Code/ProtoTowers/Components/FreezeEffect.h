#pragma once
#include "Components\IComponent.h"

class Entity;
class InfernoTimer;
class NavAgent;
class PlayerController;

class FreezeEffect : public IComponent
{
private:
	// === Variables
	InfernoTimer* m_pDamageTimer;
	NavAgent* m_pNavAgent;
	PlayerController* m_pPlayer;
	Entity* m_pEntity;
	float m_fSpeedMultiplier;
	float m_fDecreaseAmount;
	float m_fDamageRate;
	unsigned int m_uiDamage;

protected:
	// === Protected Interface === //
	virtual IComponent& operator=(IComponent& _rhs) override;
	FreezeEffect& operator=(const FreezeEffect& _rhs);
	virtual IComponent* CreateCopy(GameObject* const _gameObject, Transform* const _transform) override;
	// =========================== //

public:
	// === Constructor / Destructor === //
	FreezeEffect(GameObject* _gameObject, Transform* _transform);
	virtual ~FreezeEffect();
	// ================================ //

	// === Interface === //
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap) override;
	virtual void OnEnable() override;
	virtual void OnDisable() override;
	// ================= //

	// === Events === //
	void OnDealDamage();
	void OnRemoveEffect();
	// ============== //

	// === Accessors === //
	float GetSpeedMultipier() const;
	float GetDamageRate() const;
	unsigned int GetDamage() const;
	// ================= //

	// === Mutators === //
	void SetSpeedMultipier(float _multi);
	void SetDamageRate(float _rate);
	void SetDamage(unsigned int _damage);
	// ================ //
};

