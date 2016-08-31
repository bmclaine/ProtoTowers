#include "stdafx.h"
#include "FirstTurret.h"
#include "TowerNode.h"
#include "BaseTurret.h"
#include "EventSystem.h"

#define WAIT_AFTER_BUILD 2.0f

FirstTurret::FirstTurret(GameObject* const gameObject, Transform* const transform) : IComponent(gameObject, transform)
{
	m_iChannel = -1;
	m_pTowerNode = nullptr;
	m_itAfterTowerBuild.SetOnTimerEndFunction(std::bind(&FirstTurret::OnWaitTimeEnd, this));
}

FirstTurret::~FirstTurret()
{
}

FirstTurret& FirstTurret::operator=(const FirstTurret& rhs)
{
	return *this;
}
IComponent* FirstTurret::CreateCopy(GameObject* const gameObject, Transform* const transform)
{
	return new FirstTurret(gameObject, transform);
}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes any private, non object variables
void FirstTurret::Init() {}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes object variables
void FirstTurret::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{
	ObjectData* data = dataMap.find(GetInstanceID())->second;

	std::string propName = "Channel";
	Property* prop = data->GetProperty(propName);
	m_iChannel = atoi(prop->value.c_str());

	propName = "TowerNode";
	prop = data->GetProperty(propName);
	if (prop)
	{
		int id = atoi(prop->value.c_str());
		
		if (id > 0)
			m_pTowerNode = static_cast<GameObject*>(objectMap.find((unsigned int)id)->second)->GetComponent<TowerNode>();
	}

	if (m_pTowerNode)
		m_pTowerNode->AddForOnBuildTower(this, BindOnTowerBuiltFunction(&FirstTurret::OnTowerBuilt, this));
}

void FirstTurret::OnTowerBuilt(BaseTower*)
{
	m_itAfterTowerBuild.SetTime(WAIT_AFTER_BUILD);

	if (m_pTowerNode)
		m_pTowerNode->RemoveFromOnBuildTower(this);
}

void FirstTurret::OnWaitTimeEnd()
{
	EventSystem::GetInstance()->SendEventNow("ControlPointCaptured", (void*)m_iChannel);
	EventSystem::GetInstance()->SendEventNow("BarrierSetActiveFalse", (void*)m_iChannel);
	Inferno::Destroy(gameObject);
}