#include "stdafx.h"
#include "RaiseBarrier.h"
#include "MinionSpawner.h"

RaiseBarrier::RaiseBarrier(GameObject* const gameObject, Transform* const transform) : IComponent(gameObject, transform)
{ 
	m_pBarrierObject = nullptr;
	m_pSpawnPointObject = nullptr;
	m_pNewSpawnPoint = nullptr;
}

RaiseBarrier::~RaiseBarrier()
{
}

RaiseBarrier& RaiseBarrier::operator=(const RaiseBarrier& rhs)
{
	return *this;
}
IComponent* RaiseBarrier::CreateCopy(GameObject* const gameObject, Transform* const transform)
{
	return new RaiseBarrier(gameObject, transform);
}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes any private, non object variables
void RaiseBarrier::Init() {}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes object variables
void RaiseBarrier::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{
	ObjectData* data = dataMap.find(GetInstanceID())->second;

	std::string propName = "Barrier";
	Property* prop = data->GetProperty(propName);
	if (prop)
	{
		int id = atoi(prop->value.c_str());

		if (id > 0)
			m_pBarrierObject = static_cast<GameObject*>(objectMap.find((unsigned int)id)->second);
	}

	propName = "SpawnPoint";
	prop = data->GetProperty(propName);
	if (prop)
	{
		int id = atoi(prop->value.c_str());

		if (id > 0)
			m_pSpawnPointObject = static_cast<GameObject*>(objectMap.find((unsigned int)id)->second);
	}

	propName = "NewSpawnPoint";
	prop = data->GetProperty(propName);
	if (prop)
	{
		int id = atoi(prop->value.c_str());

		if (id > 0)
			m_pNewSpawnPoint = static_cast<GameObject*>(objectMap.find((unsigned int)id)->second)->transform;
	}

	UINT spawnerCount = 0;
	propName = "SpawnerCount";
	prop = data->GetProperty(propName);
	if (prop)
		spawnerCount = atoi(prop->value.c_str());

	m_vSpawners.reserve(spawnerCount);
	for (size_t i = 0; i < spawnerCount; i++)
	{
		propName = "Spawner" + std::to_string(i);
		prop = data->GetProperty(propName);
		if (prop)
		{
			int id = atoi(prop->value.c_str());

			if (id > 0)
				m_vSpawners.push_back((MinionSpawner*)objectMap.find((unsigned int)id)->second);
		}
	}
}

void RaiseBarrier::OnEnable()
{
	if (m_pBarrierObject)
		m_pBarrierObject->SetActive(false, true);
}

// IN: void
// OUT: void
//
// Called on the first frame that collision with a trigger is detected
void RaiseBarrier::OnTriggerEnter(Collider* col)
{
	if (col->gameObject->GetTag() != "Player")
		return;

	if (m_pBarrierObject)
		m_pBarrierObject->SetActive(true);

	if (m_pSpawnPointObject && m_pNewSpawnPoint)
		m_pSpawnPointObject->transform->SetWorldMatrix(m_pNewSpawnPoint->WorldMatrix());

	for (size_t i = 0; i < m_vSpawners.size(); i++)
		m_vSpawners[i]->SetIsActivated(false);

	Inferno::Destroy(gameObject);
}