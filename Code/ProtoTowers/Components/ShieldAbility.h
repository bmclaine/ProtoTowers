#pragma once

#include "Ability.h"

class ShieldAbility : public Ability
{
private:
	float m_fRadius;
	float m_fExpandTime;
	float m_fShieldTime;

	virtual ~ShieldAbility() override;

	ShieldAbility& operator=(const ShieldAbility& rhs);
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
	ShieldAbility(GameObject* const gameObject, Transform* const transform);

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

	friend ShieldAbility* GameObject::AddComponent();
};

