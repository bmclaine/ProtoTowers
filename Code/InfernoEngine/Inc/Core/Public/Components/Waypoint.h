///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: Inherits from IComponent.
//              These will be used by the NavAgent's to navigate.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Components\IComponent.h"

class Waypoint : public IComponent
{
private:
	Waypoint* m_pNextWaypoint;

public:
	Waypoint(GameObject* const _gameObject, Transform* const _transform);
	~Waypoint( void );

	virtual IComponent& operator=(IComponent&);
	virtual IComponent* CreateCopy(GameObject* const gameObject, Transform* const transform);
	Waypoint& operator = (const Waypoint& _assign);

	void SetNextWaypoint( Waypoint* _waypoint );
	Waypoint* GetNextWaypoint( void ) const;

	// IN: void
	// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
	//
	// Initializes object variables
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap);
};