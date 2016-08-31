#pragma once

#include "StatusEffect.h"

class MinionBuffEffect : public StatusEffect
{
private:
	float m_fHealthIncrease;
	float m_fDamageMultiplier;

	virtual ~MinionBuffEffect() override;

	MinionBuffEffect& operator=(const MinionBuffEffect& rhs);
	virtual IComponent* CreateCopy(GameObject* const gameObject, Transform* const transform) override;

public:
	MinionBuffEffect(GameObject* const gameObject, Transform* const transform);

	// IN: void
	// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
	//
	// Initializes any private, non object variables
	virtual void Init();

	// IN: void
	// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
	//
	// Initializes object variables
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap) override;

	// IN: void
	// OUT: void
	//
	// Called when the Component becaomes enabled, whether its from just the component being Enabled, or the GameObject it's attached to.
	virtual void OnEnable() override;
	// IN: void
	// OUT: void
	//
	// Called when the Component is being destroyed
	virtual void OnDestroy() override;

	void SetHealthIncrease(float health);
	void SetDamageMultiplierIncrease(float mul);
};

