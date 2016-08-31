//Author: William Zane Lewis
//
//Wave of minions to spawn after the barrier lowers

#include "stdafx.h"
#include "MinionWaveSpawn.h"
#include "EventSystem.h"
#include "MinionSpawnPoint.h"
#include "AlertMessage.h"
#include "UIElement.h"


MinionWaveSpawn::MinionWaveSpawn(GameObject* const _gameObject, Transform* const _transform) : IComponent(_gameObject, _transform)
{
	m_bSpawnMinions = false;
	m_iChannel = -2;
	m_iNumMinions = 0;
	m_iNumMinionsSpawned = 0;
	m_iNumSpawnPoint = 0;
	m_pAlertMessage = nullptr;
}

MinionWaveSpawn::~MinionWaveSpawn()
{

}

IComponent* MinionWaveSpawn::CreateCopy(GameObject* const _gameobject, Transform* const _transform)
{
	MinionWaveSpawn* newComponent = new MinionWaveSpawn(_gameobject, _transform);
	(*newComponent) = *this;

	return newComponent;
}

IComponent& MinionWaveSpawn::operator=(IComponent& _rhs)
{
	MinionWaveSpawn* rhsBT = dynamic_cast<MinionWaveSpawn*>(&_rhs);
	if (rhsBT == nullptr) {
		return *this;
	}

	return operator=(*rhsBT);
}

MinionWaveSpawn& MinionWaveSpawn::operator=(const MinionWaveSpawn& _rhs)
{
	if (this != &_rhs) {
		m_iChannel = _rhs.m_iChannel;
		m_iNumMinions = _rhs.m_iNumMinions;
		m_vMinionsToSpawn.clear();
		for (int i = 0; i < m_iNumMinions; i++)
		{
			m_vMinionsToSpawn.push_back(_rhs.m_vMinionsToSpawn[i]);
		}
		m_sMessage = _rhs.m_sMessage;
		m_bSpawnMinions = _rhs.m_bSpawnMinions;
		m_iNumMinionsSpawned = _rhs.m_iNumMinionsSpawned;
		m_itSpawnDelay = _rhs.m_itSpawnDelay;
		m_vSpawnPoints.clear();
		for (int i = 0; i < _rhs.m_vSpawnPoints.size(); i++)
		{
			m_vSpawnPoints.push_back(_rhs.m_vSpawnPoints[i]);
		}
	}

	return *this;
}

void MinionWaveSpawn::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{
	ObjectData* data = dataMap.find(GetInstanceID())->second;
	std::string propName;
	Property* prop;
	unsigned int id;
	m_vMinionsToSpawn.clear();
	m_vSpawnPoints.clear();
	propName = "Channel";
	prop = data->GetProperty(propName);
	m_iChannel = atoi(prop->value.c_str());

	propName = "MinionTypeCount";
	prop = data->GetProperty(propName);
	m_iNumMinions = atoi(prop->value.c_str());
	for (int i = 0; i < m_iNumMinions; ++i){
		propName = "WaveMinion" + std::to_string(i);
		prop = data->GetProperty(propName);
		MinionType mType = (MinionType)atoi(prop->value.c_str());
		AddMinionType(mType);
	}

	propName = "SpawnPointCount";
	prop = data->GetProperty(propName);
	m_iNumSpawnPoint = atoi(prop->value.c_str());
	for (int i = 0; i < m_iNumSpawnPoint; i++)
	{
		propName = "SpawnPoint" + std::to_string(i);
		prop = data->GetProperty(propName);
		id = (unsigned int)atoi(prop->value.c_str());
		MinionSpawnPoint* spawnPoint = (MinionSpawnPoint*)objectMap.find(id)->second;
		AddSpawnPoint(spawnPoint);
	}

}

void MinionWaveSpawn::Init()
{

}

void MinionWaveSpawn::OnEnable()
{
	EventSystem::GetInstance()->RegisterForEvent("BarrierSetActiveFalse", this, BindEventFunction(&MinionWaveSpawn::EnableSpawning, this));

	if (m_pAlertMessage == nullptr) {
		UIElement* foundElement = Inferno_UI::GetUIElementByName(std::string("Alert Message"), std::string("ExtendedHUD"));
		ASSERT(foundElement != nullptr, "No Alert Message found. It's possible it's being added after this Control Point is enabling.");
		if (foundElement != nullptr) {
			m_pAlertMessage = foundElement->GetComponent<AlertMessage>();
		}
	}
	m_bSpawnMinions = false;
	m_iNumMinionsSpawned = 0;
	IComponent::OnEnable();
}

void MinionWaveSpawn::OnDisable()
{
	EventSystem::GetInstance()->UnregisterFromEvent("BarrierSetActiveFalse", this, BindEventFunction(&MinionWaveSpawn::EnableSpawning, this));

	m_pAlertMessage = nullptr;

	IComponent::OnDisable();
}

void MinionWaveSpawn::EnableSpawning(void* data)
{
	int _channel = (int)data;
	if (m_iChannel == _channel)
	{
		if (m_pAlertMessage == nullptr) {
			UIElement* foundElement = Inferno_UI::GetUIElementByName(std::string("Alert Message"), std::string("ExtendedHUD"));
			if (foundElement != nullptr) {
				m_pAlertMessage = foundElement->GetComponent<AlertMessage>();
			}
		}

		if (m_pAlertMessage != nullptr) {
			m_sMessage = "ENEMY COUNTER ATTACK INBOUND";
			m_pAlertMessage->QueueMessage(m_sMessage, 1.5f, Color(1.0f, 0.0f, 0.0f, 1.0f), Color(1.0f, 1.0f, 1.0f, 1.0f));
		}

		Inferno::PostAudioEvent(MEGA_TANK_ALERT);

		SpawnWave();
	}
}

void MinionWaveSpawn::AddMinionType(MinionType minionType)
{
	m_vMinionsToSpawn.push_back(minionType);
}

void MinionWaveSpawn::AddSpawnPoint(MinionSpawnPoint * spawnPoint)
{
	if (spawnPoint != nullptr)
	{
		m_vSpawnPoints.push_back(spawnPoint);
	}
}

std::string MinionWaveSpawn::GetNameFromType(MinionType _type)
{
	std::string typeName;

	switch (_type) {
	case MinionType::BOSS:
		typeName = "Boss_Minion_";
		break;
	case MinionType::MINI:
		typeName = "Mini_Minion_";
		break;
	case MinionType::RANGED:
		typeName = "Ranged_Minion_";
		break;
	case MinionType::TANK:
		typeName = "Tank_Minion_";
		break;
	default:
		return std::string();
	}
	return (typeName += "Red");
	
}

void MinionWaveSpawn::SpawnWave()
{
	for (int i = 0; i < m_iNumSpawnPoint; i++)
	{
		m_vSpawnPoints[i]->gameObject->SetActive(true);
	}

	GameObject* minion;
	for (int i = 0; i < m_iNumSpawnPoint && m_iNumMinionsSpawned < m_iNumMinions; ++i)
	{
		
		minion = Inferno::Instantiate(GetNameFromType(m_vMinionsToSpawn[m_iNumMinionsSpawned]));
		minion->transform->SetPosition(m_vSpawnPoints[i]->transform->GetPosition());
		minion->transform->SetRotation(m_vSpawnPoints[i]->transform->GetRotation());
		NavAgent* navAgent = minion->GetComponent<NavAgent>();
		navAgent->SetWaypoint(m_vSpawnPoints[i]->GetStartingWaypoint());
		m_iNumMinionsSpawned++;
	}
	if (m_iNumMinionsSpawned < m_iNumMinions)
	{
		m_itSpawnDelay.Reset();
		m_itSpawnDelay.SetTime(1.5f);
		m_bSpawnMinions = true;
	}
	else
	{
		m_bSpawnMinions = false;
	}
	for (int i = 0; i < m_iNumSpawnPoint; i++)
	{
		m_vSpawnPoints[i]->gameObject->SetActive(false);
	}
}

void MinionWaveSpawn::Update()
{
	if (m_bSpawnMinions && m_itSpawnDelay.IsFinished())
	{
		SpawnWave();
	}
}