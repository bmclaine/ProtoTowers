#include "stdafx.h"
#include "HealthPickup.h"
#include "Player.h"
#include "Physics\Collider.h"
#include "GameObject.h"
#include "Components\Renderer.h"
#include "InfernoTime.h"
#include "../Public/ModuleInterface.h"
#include "Components\SpriteRenderer.h"

#define SPAWN_TIME 30.0f

HealthPickup::HealthPickup(GameObject* const _gameObject, Transform* const _transform) : IComponent(_gameObject, _transform){
	m_SpawnTimer.SetTime(SPAWN_TIME);
	m_Active = true;
	m_pSpawnIcon = nullptr;
}

HealthPickup::~HealthPickup(){

}

IComponent& HealthPickup::operator=(IComponent& _rhs) {
	HealthPickup* rhs = dynamic_cast<HealthPickup*>(&_rhs);
	if (rhs != nullptr){
		return *this;
	}

	return operator=(*rhs);
}

HealthPickup& HealthPickup::operator=(const HealthPickup& _rhs){

	if (this != &_rhs){
		m_HealPercent = _rhs.m_HealPercent;
		m_SpawnTimer = _rhs.m_SpawnTimer;
		m_fcFlags = _rhs.m_fcFlags;
	}

	return *this;
}


IComponent* HealthPickup::CreateCopy(GameObject* const _gameobject, Transform* const _transform){
	HealthPickup* newComponent = new HealthPickup(_gameobject, _transform);
	(*newComponent) = *this;
	return newComponent;
}

void HealthPickup::Init(){
	m_HealPercent = 0.25f;
	m_SpawnTimer.SetOnTimerEndFunction(BindOnTimerEndFunction(&HealthPickup::Reactivate, this));
	m_Active = true;
}

void HealthPickup::OnEnable(){
	m_Active = true;

	IComponent::OnEnable();
}

void HealthPickup::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap){
	ObjectData* data = dataMap.find(GetInstanceID())->second;

	std::string propName = "HealPercent";
	Property* prop = data->GetProperty(propName);
	m_HealPercent = (float)atof(prop->value.c_str());

	//propName = "SpawnTimer";
	//prop = data->GetProperty(propName);
	//m_SpawnTimer.SetTime((float)atof(prop->value.c_str()));

	propName = "Active";
	prop = data->GetProperty(propName);
	m_Active = atoi(prop->value.c_str()) != 0;
}

void HealthPickup::Update(){
	if (m_Active)
		transform->Rotate(Vector3(0, ToRadians(60.0f * Time::DeltaTime), 0));

	if (m_pSpawnIcon)
		m_pSpawnIcon->SetFillAmount((SPAWN_TIME - m_SpawnTimer.TimeRemaining()) / SPAWN_TIME);
}

void HealthPickup::OnTriggerEnter(Collider* _col){
	if (!m_Active) return;
	Player* player = _col->gameObject->GetComponent<Player>();
	if (player != nullptr){
		float currentHealth = player->GetHealth();
		float maxHealth = player->GetMaxHealth();
		if (currentHealth >= maxHealth) return;
		currentHealth += maxHealth * m_HealPercent;
		currentHealth = fminf(currentHealth, maxHealth);
		player->SetHealth(currentHealth);
		player->UpdateHealthBar();
		Inferno::PostAudioEvent(HEALTH_PICK_UP);
		Deactivate();
	}
}

void HealthPickup::Reactivate(){
	if (m_Active) return;
	m_Active = true;
	Collider* collider = gameObject->GetComponent<Collider>(true);
	collider->SetEnabled(true);
	Renderer* renderer = gameObject->GetComponent<Renderer>(true);
	renderer->SetEnabled(true);

	Color particleColor(0.0f, 1.0f, 0.0f, 1.0f);
	GameObject* object = Inferno::GetNewGameObject();
	object->SetName("Health Spawn Particles");
	object->transform->SetPosition(gameObject->transform->GetPosition());
	ParticleSystem* particle = object->AddComponent<ParticleSystem>();
	particle->SetShape(EMITTER_SHAPE_CIRCLE);
	particle->SetStyle(EMITTER_STYLE_EDGE);
	particle->SetSphereDimensions(0.75f);
	particle->SetEmissionRate(120);
	particle->SetLifetime(0.5f);
	particle->SetDuration(0.5f);
	particle->SetStartSpeed(3.0f);
	particle->SetStartSize(0.1f);
	particle->SetEndSize(0.1f);
	particle->SetStartColor(particleColor);
	particle->SetEndColor(particleColor);
	particle->Play();
	Inferno::Destroy(object, 0.5f);

	if (m_pSpawnIcon){
		Inferno::Destroy(m_pSpawnIcon->gameObject);
		m_pSpawnIcon = nullptr;
	}
}

void HealthPickup::Deactivate(){
	m_Active = false;
	m_SpawnTimer.Reset();
	Collider* collider = gameObject->GetComponent<Collider>(true);
	collider->SetEnabled(false);
	Renderer* renderer = gameObject->GetComponent<Renderer>(true);
	renderer->SetEnabled(false);

	Color particleColor(0.0f, 1.0f, 0.0f, 1.0f);
	GameObject* object = Inferno::GetNewGameObject();
	object->SetName("Health Spawn Particles");
	object->transform->SetPosition(gameObject->transform->GetPosition());
	ParticleSystem* particle = object->AddComponent<ParticleSystem>();
	particle->SetShape(EMITTER_SHAPE_SPHERE);
	particle->SetStyle(EMITTER_STYLE_INNER);
	particle->SetSphereDimensions(1.0f);
	particle->SetEmissionRate(120);
	particle->SetLifetime(0.5f);
	particle->SetDuration(0.5f);
	particle->SetStartSpeed(3.0f);
	particle->SetStartSize(0.1f);
	particle->SetEndSize(0.1f);
	particle->SetStartColor(particleColor);
	particle->SetEndColor(particleColor);
	particle->Play();
	Inferno::Destroy(object, 0.5f);

	GameObject* spawnIcon = Inferno::Instantiate("Health Respawn Icon");
	Vector3 pos = transform->GetPosition() + Vector3::Up() * 1.25f;
	spawnIcon->transform->SetPosition(pos);

	m_pSpawnIcon = spawnIcon->GetComponent<SpriteRenderer>();
	m_pSpawnIcon->SetFillAmount(0.0f);
}


