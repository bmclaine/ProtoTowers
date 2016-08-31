#pragma once

#include "Components\IComponent.h"
#include "InfernoTimer.h"

class DelayCredits : public IComponent
{
	InfernoTimer m_Timer;
public:
	DelayCredits(GameObject* const gameObject, Transform* const transform);
	virtual ~DelayCredits() override;

	virtual IComponent& operator=(IComponent&) override;
	virtual IComponent* CreateCopy(GameObject* const gameObject, Transform* const transform) override;

	// IN: void
	// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
	//
	// Initializes object variables
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap);

	virtual void OnEnable() override;

	virtual void Update();
};

