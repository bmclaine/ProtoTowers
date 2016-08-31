#include "stdafx.h"
#include "ResourceCrateSpawner.h"
#include "EventSystem.h"
#include "ResourceCrate.h"



ResourceCrateSpawner::ResourceCrateSpawner(GameObject* const _gameObject, Transform* const _transform) : IComponent(_gameObject,_transform)
{
	m_iSpawnChannel = -2;
	m_bHasSpawned = false;
}

ResourceCrateSpawner::~ResourceCrateSpawner()
{

}

IComponent* ResourceCrateSpawner::CreateCopy(GameObject* const _gameObject, Transform* const _transform)
{
	ResourceCrateSpawner* newComponent = new ResourceCrateSpawner(_gameObject, _transform);
	(*newComponent) = *this;

	return newComponent;
}

IComponent& ResourceCrateSpawner::operator= (IComponent& _rhs)
{
	ResourceCrateSpawner* rhsBT = dynamic_cast<ResourceCrateSpawner*>(&_rhs);
	if (rhsBT == nullptr) {
		return *this;
	}

	return operator=(*rhsBT);
}
ResourceCrateSpawner& ResourceCrateSpawner::operator= (const ResourceCrateSpawner& _rhs)
{
	if (this != &_rhs) 
	{
		m_itRespawnTimer = _rhs.m_itRespawnTimer;
	}

	return *this;
}
void ResourceCrateSpawner::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{
	ObjectData* data = dataMap.find(GetInstanceID())->second;

	std::string propertyName = "SpawnChannel";
	Property* prop = data->GetProperty(propertyName);
	m_iSpawnChannel = atoi(prop->value.c_str());

	propertyName = "RespawnTime";
	prop = data->GetProperty(propertyName);
	m_fSpawnTime = (float)atof(prop->value.c_str());
}

void ResourceCrateSpawner::Init()
{
	
}
void ResourceCrateSpawner::OnEnable()
{
	IComponent::OnEnable();
	EventSystem::GetInstance()->RegisterForEvent("StartResourceCrateRespawn", this, BindEventFunction(&ResourceCrateSpawner::StartSpawnTimer, this));
	m_bHasSpawned = false;
}

void ResourceCrateSpawner::OnDisable()
{
	IComponent::OnDisable();
	EventSystem::GetInstance()->UnregisterFromEvent("StartResourceCrateRespawn", this, BindEventFunction(&ResourceCrateSpawner::StartSpawnTimer, this));
	m_iSpawnChannel = -2;
}

void ResourceCrateSpawner::Update()
{
	if (m_itRespawnTimer.IsFinished() && !m_bHasSpawned)
	{
		SpawnNewCrate();
	}
}

void ResourceCrateSpawner::StartSpawnTimer(void * _data)
{
	int _spawnerChannel = (int)_data;
	if (m_iSpawnChannel == _spawnerChannel)
	{
		m_itRespawnTimer.Reset();
		m_itRespawnTimer.SetTime(m_fSpawnTime);
		m_bHasSpawned = false;
	}
}

void ResourceCrateSpawner::SpawnNewCrate()
{
	GameObject* _resource = Inferno::Instantiate("ResourceCrate");
	Vector3 pos = gameObject->transform->GetPosition();
	_resource->transform->SetPosition(pos);
	_resource->GetComponent<ResourceCrate>()->SetChannel(m_iSpawnChannel);
	m_bHasSpawned = true;
	GameObject* particleObject = Inferno::GetNewGameObject();
	particleObject->SetName("CrateSpawnParticles");
	particleObject->transform->SetPosition(transform->GetPosition());
	ParticleSystem* particles = particleObject->AddComponent<ParticleSystem>();
	particles->SetShape(EMITTER_SHAPE_CONE);
	particles->SetStyle(EMITTER_STYLE_INNER);
	particles->SetSphereDimensions(5.0f);
	particles->SetEmissionRate(300);
	particles->SetDuration(0.2f);
	particles->SetLifetime(0.6f);
	particles->SetStartSpeed(7.0f);
	particles->SetStartSize(0.2f);
	particles->SetEndSize(0.1f);
	particles->SetStartColor(Color( 255,  255, 255, 1));
	particles->SetEndColor(Color(255,  255,  255, 1));
	particles->Play();
	Inferno::Destroy(particleObject, particles->GetDuration());

}