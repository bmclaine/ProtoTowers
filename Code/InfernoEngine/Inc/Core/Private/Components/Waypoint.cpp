///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: Inherits from IComponent.
//              These will be used by the NavAgent's to navigate.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "Components\Waypoint.h"

Waypoint::Waypoint(GameObject* const _gameObject, Transform* const _transform)
	: IComponent(_gameObject, _transform)
	, m_pNextWaypoint(nullptr)
{ }

Waypoint::~Waypoint()
{ }

IComponent& Waypoint::operator=(IComponent& _assign)
{
	Waypoint* temp = (Waypoint*)&_assign;
	if (temp == nullptr) {
		return *this;
	}
	return operator=(*temp);
}

IComponent* Waypoint::CreateCopy(GameObject* const _gameObject, Transform* const _transform)
{
	Waypoint* temp = new Waypoint(_gameObject, _transform);
	*temp = *this;

	return temp;
}

Waypoint& Waypoint::operator = (const Waypoint& _assign)
{
	if (this != &_assign)
	{
		m_pNextWaypoint = _assign.m_pNextWaypoint;
		m_fcFlags = _assign.m_fcFlags;
	}
	return *this;
}

void Waypoint::SetNextWaypoint(Waypoint* _waypoint)
{
	m_pNextWaypoint = _waypoint;
}

Waypoint* Waypoint::GetNextWaypoint() const
{
	return m_pNextWaypoint;
}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes object variables
void Waypoint::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{
	// What the fuck did you just say to me?
	ObjectData* data = dataMap.find(GetInstanceID())->second;

	Property* prop = data->GetProperty(std::string("Next"));
	int id = atoi(prop->value.c_str());
	if (id > 0){
		m_pNextWaypoint = (Waypoint*)objectMap.find((unsigned int)id)->second;
	}
}

