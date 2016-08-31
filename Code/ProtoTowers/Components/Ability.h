#pragma once

#include "Components\IComponent.h"
#include "InfernoTimer.h"
#include "Color.h"

class PlayerController;

class Ability : public IComponent
{
protected:
	InfernoTimer m_itCastTimer;
	InfernoTimer m_itCooldownTimer;
	Color m_cActiveTimerColor;
	float m_fCastTime;
	float m_fCooldown;
	PlayerController* m_pController;
	Texture* m_pTexture;
	bool m_bCanCast;

	Ability(GameObject* const gameObject, Transform* const transform);
	virtual ~Ability() override;

	Ability& operator=(const Ability& rhs);
	virtual IComponent* CreateCopy(GameObject* const gameObject, Transform* const transform) = 0;

public:
	// IN: void
	// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
	//
	// Initializes any private, non object variables
	virtual void Init();

	// IN: void
	// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
	//
	// Initializes object variables
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap) = 0;

	// IN: void
	// OUT: void
	//
	// Uses the ability
	virtual void UseAbility() = 0;
	// IN: void
	// OUT: void
	//
	// Reenables controls that were disabled
	virtual void ReenableControls();

	// IN: bool - To show the indicator or not
	// OUT: void
	//
	// Sets the Indicator Circles display status to the incoming bool
	virtual void DisplayIndicatorCircle(bool display);

	void SetActiveTimerColor(Color color);
	void SetCastTime(float time);
	void SetCooldown(float time);
	void SetTexture(Texture* texture);

	Color GetActiveTimerColor() const;
	float GetCastTime() const;
	float GetCastTimeRemaining() const;
	float GetCooldown() const;
	float GetCooldownRemaining() const;
	Texture* GetTexture() const;
	bool  CanCast() const;
};

