#include "stdafx.h"
#include "MinionSpawnPoint.h"

// ===== Constructor / Destructor ===== //
MinionSpawnPoint::MinionSpawnPoint(GameObject* _gameObject, Transform* _transform) : IComponent(_gameObject, _transform)
{
	m_pStartingWaypoint = nullptr;
}

MinionSpawnPoint::~MinionSpawnPoint()
{

}
// ==================================== //

// ===== Interface ===== //
void MinionSpawnPoint::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{
	ObjectData* data = dataMap.find(GetInstanceID())->second;

	std::string propName = std::string("Waypoint");
	Property* prop = data->GetProperty(propName);

	unsigned int objectID = (unsigned int)atoi(prop->value.c_str());
	if (objectID != 0) {
		SetStartingWaypoint((Waypoint*)objectMap.find(objectID)->second);
	}
}
// ===================== //

// ===== Protected Interface ===== //
IComponent& MinionSpawnPoint::operator=(IComponent& _rhs)
{
	MinionSpawnPoint* rhsMSP = dynamic_cast<MinionSpawnPoint*>(&_rhs);
	if (rhsMSP == nullptr) {
		return *this;
	}

	return operator=(*rhsMSP);
}

MinionSpawnPoint& MinionSpawnPoint::operator=(const MinionSpawnPoint& _rhs)
{
	if (this != &_rhs) {
		m_pStartingWaypoint = _rhs.m_pStartingWaypoint;
	}

	return *this;
}

IComponent* MinionSpawnPoint::CreateCopy(GameObject* const _gameObject, Transform* const _transform)
{
	MinionSpawnPoint* newMSP = new MinionSpawnPoint(_gameObject, _transform);
	*newMSP = *this;

	return newMSP;
}
// =============================== //

// ===== Mutators ===== //
void MinionSpawnPoint::SetStartingWaypoint(Waypoint* _waypoint)
{
	m_pStartingWaypoint = _waypoint;
}
// ==================== //

// ===== Accessors ===== //
Waypoint* MinionSpawnPoint::GetStartingWaypoint() const
{
	return m_pStartingWaypoint;
}
// ===================== //