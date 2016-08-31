#include "stdafx.h"
#include "FirstTurretUpgrade.h"
#include "EventSystem.h"

FirstTurretUpgrade::FirstTurretUpgrade(GameObject* const gameObject, Transform* const transform) : IComponent(gameObject, transform)
{
	m_iChannel = -1;
}

FirstTurretUpgrade::~FirstTurretUpgrade()
{
}

FirstTurretUpgrade& FirstTurretUpgrade::operator=(const FirstTurretUpgrade& rhs)
{
	return *this;
}
IComponent* FirstTurretUpgrade::CreateCopy(GameObject* const gameObject, Transform* const transform)
{
	return new FirstTurretUpgrade(gameObject, transform);
}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes any private, non object variables
void FirstTurretUpgrade::Init() {}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes object variables
void FirstTurretUpgrade::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{
	ObjectData* data = dataMap.find(GetInstanceID())->second;

	std::string propertyName = "Channel";
	Property* prop = data->GetProperty(propertyName);
	m_iChannel = atoi(prop->value.c_str());
}

void FirstTurretUpgrade::OnEnable()
{
	EventSystem::GetInstance()->RegisterForEvent("UpgradeTowers", this, std::bind(&FirstTurretUpgrade::OnTowerUpgrade, this, std::placeholders::_1));
}
void FirstTurretUpgrade::OnDisable()
{
	EventSystem::GetInstance()->UnregisterFromEvent("UpgradeTowers", this, std::bind(&FirstTurretUpgrade::OnTowerUpgrade, this, std::placeholders::_1));
}

// Events
void FirstTurretUpgrade::OnTowerUpgrade(void* pData)
{
	UINT team = *(UINT*)pData;

	if (team == BLUE)
	{
		EventSystem::GetInstance()->SendEventNow("BarrierSetActiveFalse", (void*)m_iChannel);
		EventSystem::GetInstance()->SendEventNow("ControlPointCaptured", (void*)m_iChannel);
		Inferno::Destroy(gameObject);
	}
}