//	Author: Eddie Goodman
//
//	The base class used for every gameobject and component

#pragma once

#include <map>
#include <ObjectData.h>

class Object{

private:
	unsigned int m_uiInstanceID;

public:
	Object() = default;
	virtual ~Object() = default;
	
	// IN: void
	// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
	//
	// Initializes any private, non object variables
	virtual void Init() = 0;

	// IN: void
	// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
	//
	// Initializes object variables
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap) = 0;

	// IN: void
	// OUT: unsigned int - unique instance id of the object
	//
	// Returns the unique instance id of the object
	unsigned int GetInstanceID() const;

	void SetInstanceID(unsigned int);
	
};