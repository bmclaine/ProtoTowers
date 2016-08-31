#pragma once

#include "Ability.h"
#include "GameObject.h"

class WeaponBuffAbility : public Ability
{
	float m_fFireRateIncrease;
	float m_fDuration;

	virtual ~WeaponBuffAbility() override;

	WeaponBuffAbility& operator=(const WeaponBuffAbility& rhs);
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
	WeaponBuffAbility(GameObject* const gameObject, Transform* const transform);

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

	virtual void OnEnable() override;

	// IN: void
	// OUT: void
	//
	// Uses the ability
	virtual void UseAbility() override;

	friend WeaponBuffAbility* GameObject::AddComponent();
};