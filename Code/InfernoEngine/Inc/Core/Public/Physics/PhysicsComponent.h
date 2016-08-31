#pragma once

#include "Components\IComponent.h"

class PhysicsComponent : public IComponent{

protected:
	virtual ~PhysicsComponent() override = default;

public:
	PhysicsComponent(GameObject* const gameObject, Transform* const transform);

	virtual IComponent* CreateCopy(GameObject* const gameObject, Transform* const transform) = 0;

	// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
	//
	// Initializes any private, non object variables
	virtual void Init() = 0;

	// IN: void
	// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
	//
	// Initializes object variables
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap) = 0;
};