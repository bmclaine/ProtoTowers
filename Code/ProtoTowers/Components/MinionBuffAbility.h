#pragma once

#include "Ability.h"

class MinionBuffAbility : public Ability
{
	float m_fBuffTime;
	float m_fRadius;
	float m_fHealthIncrease;
	float m_fDamageMultiplier;
	GameObject* m_pIndicatorCircle;

	virtual ~MinionBuffAbility() override;

	MinionBuffAbility& operator=(const MinionBuffAbility& rhs);
	virtual IComponent* CreateCopy(GameObject* const gameObject, Transform* const transform) override;

	// IN: void
	// OUT: void
	//
	// To be called when the cast time is finished
	void OnCastFinished();
	// IN: void
	// OUT: void
	//
	// Called when the cooldown timer expires
	void OnCooldownFinished();

public:
	MinionBuffAbility(GameObject* const gameObject, Transform* const transform);

	// IN: void
	// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
	//
	// Initializes any private, non object variables
	virtual void Init() override;

	// IN: void
	// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
	//
	// Initializes object variables
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap) override;

	virtual void OnEnable() override;

	// IN: void
	// OUT: void
	//
	// Uses the ability
	virtual void UseAbility() override;

	// IN: bool - To show the indicator or not
	// OUT: void
	//
	// Sets the Indicator Circles display status to the incoming bool
	virtual void DisplayIndicatorCircle(bool display) override;

	float GetBuffTime() const;

	void SetBuffTime(float time);
};

