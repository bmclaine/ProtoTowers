///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author:      Doug Berg
//
// Description: Abstract Base Class component for all AI based components.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

// Engine Includes
#include "IComponent.h"

// Forward Declarations
class AIModule;
class GameObject;



class AIComponent : public IComponent
{
private:


public:
	AIComponent(GameObject* const _gameObject, Transform* const _transform );
	virtual ~AIComponent( void ) = 0;

	// IN: void
	// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
	//
	// Initializes object variables
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap) = 0;
};

