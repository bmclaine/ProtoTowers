//Author: William Zane Lewis
//
//An environment object that spawns in resources when it is destroyed
#include "stdafx.h"
#include "ResourceCrate.h"
#include "Resource.h"
#include "BaseProjectile.h"
#include "EventSystem.h"



ResourceCrate::ResourceCrate(GameObject* const _gameObject, Transform* const _transform) : IComponent(_gameObject, _transform)
{
	m_bIsDead = false;
	m_iSpawnChannel = -1;
}

ResourceCrate::~ResourceCrate()
{

}



IComponent* ResourceCrate::CreateCopy(GameObject* const _gameObject, Transform* const _transform)
{
	ResourceCrate* newComponent = new ResourceCrate(_gameObject, _transform);
	(*newComponent) = *this;

	return newComponent;
}

void ResourceCrate::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap){

}

void ResourceCrate::OnEnable()
{

	IComponent::OnEnable();
}

void ResourceCrate::OnDisable()
{
	m_iSpawnChannel = -1;
	IComponent::OnDisable();
}

void ResourceCrate::OnTriggerEnter(Collider* _col)
{
	BaseProjectile* bProj = _col->gameObject->GetComponent<BaseProjectile>();
	if (bProj != nullptr && !m_bIsDead)
	{
		m_bIsDead = true;
		SpawnInResources();

		EventSystem::GetInstance()->SendEventNow("StartResourceCrateRespawn", (void*)m_iSpawnChannel);

		GameObject* particleObject = Inferno::GetNewGameObject();
		particleObject->SetName("ExplosionParticles");
		particleObject->transform->SetPosition(transform->GetPosition());
		ParticleSystem* particles = particleObject->AddComponent<ParticleSystem>();
		particles->SetShape(EMITTER_SHAPE_SPHERE);
		particles->SetStyle(EMITTER_STYLE_INNER);
		particles->SetSphereDimensions(5.0f);
		particles->SetEmissionRate(300);
		particles->SetDuration(0.2f);
		particles->SetLifetime(0.6f);
		particles->SetStartSpeed(7.0f);
		particles->SetStartSize(0.2f);
		particles->SetEndSize(0.1f);
		particles->SetStartColor(Color(210 / 255, 180 / 255, 140 / 255, 1));
		particles->SetEndColor(Color(210 / 255, 180 / 255, 140 / 255, 1));
		particles->Play();

		Inferno::PostAudioEventAtObjectPosition(CRATE_BREAK,gameObject);
		Inferno::Destroy(particleObject, particles->GetDuration());
		Inferno::Destroy(gameObject);
	}
}

void ResourceCrate::OnCollisionEnter(Collider* _col){
	BaseProjectile* bProj = _col->gameObject->GetComponent<BaseProjectile>();
	if (bProj != nullptr && !m_bIsDead)
	{
		m_bIsDead = true;
		SpawnInResources();

		GameObject* particleObject = Inferno::GetNewGameObject();
		particleObject->SetName("ExplosionParticles");
		particleObject->transform->SetPosition(transform->GetPosition());
		ParticleSystem* particles = particleObject->AddComponent<ParticleSystem>();
		particles->SetShape(EMITTER_SHAPE_SPHERE);
		particles->SetStyle(EMITTER_STYLE_INNER);
		particles->SetSphereDimensions(5.0f);
		particles->SetEmissionRate(300);
		particles->SetDuration(0.2f);
		particles->SetLifetime(0.6f);
		particles->SetStartSpeed(7.0f);
		particles->SetStartSize(0.2f);
		particles->SetEndSize(0.1f);
		particles->SetStartColor(Color(210 / 255, 180 / 255, 140 / 255, 1));
		particles->SetEndColor(Color(210 / 255, 180 / 255, 140 / 255, 1));
		particles->Play();

		Inferno::PostAudioEventAtObjectPosition(CRATE_BREAK, gameObject);
		Inferno::Destroy(particleObject, particles->GetDuration());
		Inferno::Destroy(gameObject);
	}
}

void ResourceCrate::Update()
{

	
}

void ResourceCrate::SpawnInResources()
{
	for (int i = 0; i < NUM_RESOURCES_TO_SPAWN; i++)
	{
		GameObject* _resource = Inferno::Instantiate("Resource");
		_resource->transform->SetPosition(gameObject->transform->GetPosition());
		Vector3 offset = _resource->transform->GetPosition();
		offset.x += (float)i;
		offset.z += (float)i;
		offset.y = .4f;
		_resource->transform->SetPosition(offset);
	//	Vector3 vel;
	//	float xOffset = RandFloat(-1.0f, 1.0f);
	//	float force = RandFloat(300.0f, 400.0f);
	//	float zOffset = RandFloat(-1.0f, 1.0f);
	//	vel.x = xOffset * force;
	//	vel.y = 1 * force;
	//	vel.z = zOffset * force;
	//	Rigidbody * rigidBody = _resource->GetComponent<Rigidbody>();
	//	if (rigidBody != nullptr)
	//		rigidBody->SetVelocity(vel);
	}
}

IComponent& ResourceCrate::operator=(IComponent& _rhs)
{
	ResourceCrate* rhsBT = dynamic_cast<ResourceCrate*>(&_rhs);
	if (rhsBT == nullptr) {
		return *this;
	}

	return operator=(*rhsBT);
}

ResourceCrate& ResourceCrate::operator=(const ResourceCrate& _rhs)
{
	if (this != &_rhs) {
		m_fcFlags = _rhs.m_fcFlags;
		m_bIsDead = _rhs.m_bIsDead;

	}

	return *this;
}

void ResourceCrate::SetChannel(int _spawnChannel)
{
	m_iSpawnChannel = _spawnChannel;
}
