//*********************************************************************//
//	Author:			Brian McLaine
//	Description:	
//*********************************************************************//

#include "Components\CutSceneWaypoint.h"

// === DEBUG
#include "GameObject.h"

CutSceneWaypoint::CutSceneWaypoint(GameObject* const _gameObject, Transform* const _transform) : IComponent(_gameObject, _transform)
{
	m_pNextWaypoint = nullptr;
	m_pTangetWaypoint = nullptr;
	m_fTime = 0.0f;
}

CutSceneWaypoint::~CutSceneWaypoint()
{

}

IComponent& CutSceneWaypoint::operator=(IComponent& _rhs)
{
	CutSceneWaypoint* temp = (CutSceneWaypoint*)&_rhs;

	if (temp == nullptr)
		return *this;

	return operator=(*temp);
}

IComponent* CutSceneWaypoint::CreateCopy(GameObject* const _gameObject, Transform* const _transform)
{
	CutSceneWaypoint* temp = new CutSceneWaypoint(_gameObject, _transform);

	*temp = *this;

	return temp;
}

CutSceneWaypoint& CutSceneWaypoint::operator=(const CutSceneWaypoint& _rhs)
{
	if (this != &_rhs)
	{
		m_pNextWaypoint = _rhs.m_pNextWaypoint;
		m_fTime = _rhs.m_fTime;
		m_fcFlags = _rhs.m_fcFlags;
	}

	return *this;
}

void CutSceneWaypoint::Init()
{

}

void CutSceneWaypoint::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{
	ObjectData* data = dataMap.find(GetInstanceID())->second;

	Property* prop = data->GetProperty(std::string("Next"));
	int id = atoi(prop->value.c_str());
	if (id > 0)
		m_pNextWaypoint = (CutSceneWaypoint*)objectMap.find((unsigned int)id)->second;

	prop = data->GetProperty(std::string("Tangent"));
	id = atoi(prop->value.c_str());
	if (id > 0)
		m_pTangetWaypoint = (CutSceneWaypoint*)objectMap.find((unsigned int)id)->second;

	prop = data->GetProperty(std::string("Time"));
	float time = (float)atof(prop->value.c_str());
	m_fTime = time;
}

void CutSceneWaypoint::OnDestroy()
{
	m_pNextWaypoint = nullptr;
	m_pTangetWaypoint = nullptr;
	m_fTime = 0.0f;
}

void CutSceneWaypoint::SetNextWaypoint(CutSceneWaypoint* _waypoint)
{
	m_pNextWaypoint = _waypoint;
}

CutSceneWaypoint* CutSceneWaypoint::GetNextWaypoint() const
{
	return m_pNextWaypoint;
}

CutSceneWaypoint* CutSceneWaypoint::GetTangentWaypoint() const
{
	return m_pTangetWaypoint;
}

float CutSceneWaypoint::GetWaypointTime() const
{
	return m_fTime;
}