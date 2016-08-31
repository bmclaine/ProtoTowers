#pragma once

#include "Components\IComponent.h"
#include "InfernoTimer.h"

class StatusEffect : public IComponent
{
protected:
	InfernoTimer m_itEffectTime;

	StatusEffect(GameObject* const gameObject, Transform* const transform);
	virtual ~StatusEffect() override;

	StatusEffect& operator=(const StatusEffect& rhs);
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
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap) override = 0;

	// IN: void
	// OUT: void
	//
	// Called whenever the effect needs to be removed
	virtual void RemoveEffect();

	void SetEffectTime(float time);
};

