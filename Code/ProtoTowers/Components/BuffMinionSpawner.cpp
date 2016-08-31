#include "stdafx.h"
#include "BuffMinionSpawner.h"
#include "MinionSpawnPoint.h"
#include "AIEntity.h"
#include "AlertMessage.h"
#include "EventSystem.h"
#include "UIElement.h"
#include "CutScene.h"
#include "Components\Camera.h"

#define SPAWN_TIMER 2.0f
#define ACTIVATE_TIMER 420.0f

#define MINION_MAX_HEALTH 450.0f
#define MINION_DAMAGE_MUTILIPLIER 2.0f

#define ROTATION_SPEED 90.0f

BuffMinionSpawner::BuffMinionSpawner(GameObject* const gameObject, Transform* const transform) : IComponent(gameObject, transform){
	m_bActive = false;
	m_fActivateTimer = m_fSpawnTimer = m_fMinionDamageMultiplier = m_fMinionMaxHealth = 0.0f;
	m_pAlertMessage = nullptr;
	m_pActivateIcon = nullptr;
	m_pCrystal = nullptr;
	m_pCutSceneCamera = nullptr;
	m_pCutSceneWaypoint = nullptr;
	m_pMainCamera = nullptr;
}

BuffMinionSpawner::~BuffMinionSpawner(){

}

BuffMinionSpawner& BuffMinionSpawner::operator=(const BuffMinionSpawner& _rhs){

	if (this != &_rhs){
		m_fActivateTimer = _rhs.m_fActivateTimer;
		m_fSpawnTimer = _rhs.m_fSpawnTimer;
		m_fcFlags = _rhs.m_fcFlags;
		m_fMinionDamageMultiplier = _rhs.m_fMinionDamageMultiplier;
		m_fMinionMaxHealth = _rhs.m_fMinionMaxHealth;
	}

	return *this;
}

IComponent* BuffMinionSpawner::CreateCopy(GameObject* const gameobject, Transform* const transform){
	BuffMinionSpawner* bms = new BuffMinionSpawner(gameObject, transform);
	(*bms) = (*this);
	return bms;
}

void BuffMinionSpawner::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap){
	ObjectData* data = dataMap.find(GetInstanceID())->second;

	std::string propName = "ActivateTimer";
	Property* prop = data->GetProperty(propName);
	m_fActivateTimer = prop != nullptr ? (float)atof(prop->value.c_str()) : ACTIVATE_TIMER;

	propName = "SpawnTimer";
	prop = data->GetProperty(propName);
	m_fSpawnTimer = prop != nullptr ? (float)atof(prop->value.c_str()) : SPAWN_TIMER;

	propName = "DamageMultiplier";
	prop = data->GetProperty(propName);
	m_fMinionDamageMultiplier = prop != nullptr ? (float)atof(prop->value.c_str()) : MINION_DAMAGE_MUTILIPLIER;

	propName = "MaxHealth";
	prop = data->GetProperty(propName);
	m_fMinionMaxHealth = prop != nullptr ? (float)atof(prop->value.c_str()) : MINION_MAX_HEALTH;

	m_vSpawnPoints.clear();

	propName = "SpawnerCount";
	prop = data->GetProperty(propName);
	int spawnerCount = prop != nullptr ? atoi(prop->value.c_str()) : 0;
	for (int i = 0; i < spawnerCount; ++i){
		propName = "Spawner" + std::to_string(i);
		prop = data->GetProperty(propName);
		unsigned int id = (unsigned int)atoi(prop->value.c_str());
		MinionSpawnPoint* spawnPoint = (MinionSpawnPoint*)objectMap.find(id)->second;
		m_vSpawnPoints.push_back(spawnPoint);
	}

	propName = "CutSceneWaypoint";
	prop = data->GetProperty(propName);
	unsigned int id = (unsigned int)atoi(prop->value.c_str());
	if (id != 0)
		m_pCutSceneWaypoint = (CutSceneWaypoint*)objectMap.find(id)->second;

	propName = "Camera";
	prop = data->GetProperty(propName);
	id = (unsigned int)atoi(prop->value.c_str());
	if (id != 0)
		m_pCutSceneCamera = (Camera*)objectMap.find(id)->second;

	propName = "MainCamera";
	prop = data->GetProperty(propName);
	id = (unsigned int)atoi(prop->value.c_str());
	if (id != 0)
		m_pMainCamera = (Camera*)objectMap.find(id)->second;

	m_fMinionDamageMultiplier = 3.0f;
	m_fMinionMaxHealth = 700.0f;
}

void BuffMinionSpawner::OnEnable(){
	m_ActivateTimer.SetTime(m_fActivateTimer);
	m_ActivateTimer.SetOnTimerEndFunction(BindOnTimerEndFunction(&BuffMinionSpawner::Activate, this));
	FindAlertMessage();

	GameObject* activateIcon = Inferno::Instantiate("Buff Minion Spawner Icon");
	Vector3 pos = transform->GetPosition() + Vector3::Up() * 4.25f;
	activateIcon->transform->SetPosition(pos);

	m_pActivateIcon = activateIcon->GetComponent<SpriteRenderer>();
	m_pActivateIcon->SetFillAmount(0.0f);

	if (!m_pCrystal)
		m_pCrystal = transform->GetChildByName(std::string("Crystal"))->transform;

}

void BuffMinionSpawner::OnDisable(){
	m_pAlertMessage = nullptr;

	m_SpawnTimer.FullReset();
}

void BuffMinionSpawner::OnDestroy(){
	m_pAlertMessage = nullptr;

	m_SpawnTimer.FullReset();
}

void BuffMinionSpawner::Update(){

	if (m_pActivateIcon){
		float fill = (m_fActivateTimer - m_ActivateTimer.TimeRemaining()) / m_fActivateTimer;
		m_pActivateIcon->SetFillAmount(fill);
	}

	if (m_pCrystal)
		m_pCrystal->Rotate(Vector3(0.0f, ToRadians(ROTATION_SPEED * Time::DeltaTime), 0.0f));
}

void BuffMinionSpawner::OnTriggerEnter(Collider* _collider)
{
	if (_collider->IsTrigger() || _collider->gameObject->GetName() != "Player") {
		return;
	}

	EventSystem::GetInstance()->SendEventNow("ToolTip_BuffSpawner", nullptr);
}

void BuffMinionSpawner::Activate(){
	m_SpawnTimer.SetTime(m_fSpawnTimer);
	m_SpawnTimer.SetOnTimerEndFunction(BindOnTimerEndFunction(&BuffMinionSpawner::SpawnMinions, this));
	m_SpawnTimer.Reset();
	m_bActive = true;
	SendAlert();

	StartCutScene();

	if (m_pActivateIcon){
		Inferno::Destroy(m_pActivateIcon->gameObject);
		m_pActivateIcon = nullptr;
	}

	GameObject* object = Inferno::Instantiate("Spawner Activate Particles");
	object->transform->SetPosition(transform->GetPosition());

	ParticleSystem* particles = object->GetComponent<ParticleSystem>();
	particles->Play();

	Inferno::Destroy(object, particles->GetDuration());
}

void BuffMinionSpawner::StartCutScene(){
	GameObject* cutSceneObj = Inferno::Instantiate("CutScene");
	CutScene* cutScene = cutSceneObj->GetComponent<CutScene>();
	cutScene->SetCutSceneCamera(m_pCutSceneCamera);
	cutScene->SetStartWaypoint(m_pCutSceneWaypoint);
	cutScene->SetIsLooping(false);
	cutScene->StartCutScene();
	cutScene->SetOnCutSceneEndFunction(BindOnCutSceneEndFunction(&BuffMinionSpawner::EndCutScene, this));
}

void BuffMinionSpawner::SendAlert(){
	Inferno::PostAudioEvent(MEGA_TANK_ALERT);
	if (!m_pAlertMessage) return;

	m_pAlertMessage->SetFlashColors(Color(1.0f, 0.0f, 0.0f, 1.0f), Color(1.0f, 0.0f, 0.30f, 1.0f));
	m_pAlertMessage->DisplayMessage(std::string("Super Minions Activated!"), 1.5f, true);
}

void BuffMinionSpawner::SpawnMinions(){
	if (!m_bActive) return;

	Inferno::PostAudioEventAtObjectPosition(DEFAULT_MINION_SPAWN, gameObject);

	GameObject* minion = nullptr;
	GameObject* particles = nullptr;
	NavAgent* navAgent = nullptr;

	for (unsigned int i = 0; i < m_vSpawnPoints.size(); ++i){
		minion = Inferno::Instantiate("Buff_Mini_Minion_Red");
		navAgent = minion->GetComponent<NavAgent>();
		navAgent->SetWaypoint(m_vSpawnPoints[i]->GetStartingWaypoint());
		minion->transform->SetPosition(m_vSpawnPoints[i]->transform->GetPosition());
		minion->transform->SetRotation(m_vSpawnPoints[i]->transform->GetRotation());

		particles = Inferno::Instantiate("Minion Spawn Particles");
		particles->transform->SetPosition(m_vSpawnPoints[i]->transform->GetPosition());
		Inferno::Destroy(particles, 0.75f);

		BuffMinion(minion);
	}

	m_SpawnTimer.Reset();
}

void BuffMinionSpawner::BuffMinion(GameObject* minion){
	AIEntity* entity = minion->GetComponent<AIEntity>();
	entity->SetMaxHealth(m_fMinionMaxHealth);
	entity->SetHealth(m_fMinionMaxHealth);
	entity->SetDamageMultiplier(m_fMinionDamageMultiplier);
	entity->SetAttackTimer((unsigned int)400);
	entity->SetIsBuffed(true);

	Animator* anim = minion->GetComponentInChildren<Animator>();
	anim->SetSpeed(1.5f);

	NavAgent* agent = minion->GetComponent<NavAgent>();
	agent->SetSpeed(6.2f);

	SkinnedMeshRenderer* rend = minion->GetComponentInChildren<SkinnedMeshRenderer>();
	rend->SetColor(Color(0.0f, 0.0f, 1.0f, 1.0f));

	minion->transform->SetScale(1.5f, 1.5f, 1.5f);
}

void BuffMinionSpawner::FindAlertMessage(){
	if (m_pAlertMessage == nullptr) {
		UIElement* element = Inferno_UI::GetUIElementByName(std::string("Alert Message"), std::string("ExtendedHUD"));
		if (element != nullptr) {
			m_pAlertMessage = element->GetComponent<AlertMessage>();
		}
	}
}

void BuffMinionSpawner::EndCutScene(){
	if (m_pMainCamera)
		CameraManager::SetMainCamera(m_pMainCamera);
}


