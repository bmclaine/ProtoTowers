#pragma once

#include "Components\IComponent.h"

class LookAtPlayer : public IComponent
{
	GameObject* m_pPlayer;

public:
	LookAtPlayer(GameObject* const gameObject, Transform* const transform);
	virtual ~LookAtPlayer();

	virtual IComponent& operator=(IComponent&) override;
	virtual IComponent* CreateCopy(GameObject* const gameObject, Transform* const transform) override;

	// IN: void
	// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
	//
	// Initializes object variables
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap);

	virtual void Update() override;

	void SetPlayer(GameObject* player);
};

