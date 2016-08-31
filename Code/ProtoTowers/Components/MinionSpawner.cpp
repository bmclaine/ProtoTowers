#include "stdafx.h"
#include "MinionSpawner.h"

#include "../Core/WWise_IDs.h"
#include "AlertMessage.h"
#include "Assets\Material.h"
#include "BossMinion.h"
#include "Components\NavAgent.h"
#include "Components\Waypoint.h"
#include "Engine.h"
#include "EventSystem.h"
#include "MiniMinion.h"
#include "MinionSpawnPoint.h"
#include "UIElement.h"
#include "UIComponents\UIRenderer.h"
#include "UIComponents\RectTransform.h"
#include "Assets\Texture.h"

// === Defines
#define DEACTIVATED_TIME 45.0f
#define DEACTIVATED_SPAWNCOUNT 2
#define DEACTIVATED_SPAWNDELAY_MULTIPLIER 1.3f
#define INITIAL_SPAWN_DELAY 2.0f
#define MINION_HEALTH_MULTIPLIER 0.1f

// === Static Variables
BossEvent* MinionSpawner::s_pBossEvent = nullptr;

// ===== Constructor / Destructor ===== //
MinionSpawner::MinionSpawner(GameObject* _owner, Transform* _transform) : BaseTower(_owner, _transform), m_fcMinionSpawnerFlags((unsigned int)MinionSpawnerFlags::MS_MAX_FLAGS)
{
	m_pSpawnTimer = new InfernoTimer();
	m_pDeactivateTimer = new InfernoTimer();
	m_pDeactivateTimer->SetOnTimerEndFunction(BindOnTimerEndFunction(&MinionSpawner::OnActivate, this));

	m_pAlertMessage = nullptr;
	m_pBossSpawn = nullptr;
	m_pDeactivatedIconSprite = nullptr;

	m_uiCurrentWave = m_uiMinionsSpawned = 0;
	m_fcMinionSpawnerFlags.SetFlag((unsigned int)MinionSpawnerFlags::MS_SpawnMinions_Flag, (unsigned int)true);
	m_iChannel = -2;

	m_fMaxHealth = m_fHealth = 220.0f;
	m_fInitialSpawnDelay = INITIAL_SPAWN_DELAY;

	// MiniMap Varibale Initialization
	m_pElementBossSpawnPing = nullptr;
	m_pRendererBossSpawnPing = nullptr;
	m_pTransformBossSpawnPing = nullptr;

	m_pIconBossSpawnPing = new Texture();
	m_pIconBossSpawnPing->SetImage(Inferno::GetImage(L"IconPing.dds"));

	m_pElementUnderAttackPing = nullptr;
	m_pRendererUnderAttackPing = nullptr;
	m_pTransformUnderAttackPing = nullptr;

	m_pIconUnderAttackPing = new Texture();
	m_pIconUnderAttackPing->SetImage(Inferno::GetImage(L"IconPing.dds"));

	m_pTransformBossMinion = nullptr;

	m_fMaxPingSize = 30.0f;
	m_fMinPingSize = 20.0f;

	m_fPingTime = 6.0f;

	m_pShieldDisableParticle = nullptr;
	m_pShield = nullptr;
	m_fcMinionSpawnerFlags.SetFlag((unsigned int)MinionSpawnerFlags::MS_BossSpawnPing_Flag, (unsigned int)true);
	m_fcMinionSpawnerFlags.SetFlag((unsigned int)MinionSpawnerFlags::MS_UnderAttackPing_Flag, (unsigned int)true);
	m_fcMinionSpawnerFlags.SetFlag((unsigned int)MinionSpawnerFlags::MS_Vunerable_Flag, (unsigned int)true);
}

MinionSpawner::~MinionSpawner()
{ 
	if (isEnabled()) {
		OnDisable();
	}

	// Delete And Clear MiniMap Variables
	delete m_pIconBossSpawnPing;
	delete m_pIconUnderAttackPing;
	delete m_pElementBossSpawnPing;
	delete m_pElementUnderAttackPing;
	m_pElementBossSpawnPing = nullptr;
	m_pRendererBossSpawnPing = nullptr;
	m_pTransformBossSpawnPing = nullptr;
	m_pElementUnderAttackPing = nullptr;
	m_pRendererUnderAttackPing = nullptr;
	m_pTransformUnderAttackPing = nullptr;
	m_pTransformBossMinion = nullptr;

	delete m_pSpawnTimer;
	delete m_pDeactivateTimer;
}
// ==================================== //

// ===== Interface ===== //
void MinionSpawner::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap){

	BaseTower::PostInit(objectMap, dataMap);

	ObjectData* data = dataMap.find(GetInstanceID())->second;
	std::string propName;
	Property* prop;

	// === Load in the Spawn Points
	m_vSpawnPoints.clear();

	propName = "SpawnPointCount";
	prop = data->GetProperty(propName);
	int spawnLocationCount = atoi(prop->value.c_str());
	for (int i = 0; i < spawnLocationCount; ++i){
		propName = "SpawnPoint" + std::to_string(i);
		prop = data->GetProperty(propName);
		unsigned int id = (unsigned int)atoi(prop->value.c_str());
		MinionSpawnPoint* spawnPoint = (MinionSpawnPoint*)objectMap.find(id)->second;
		AddSpawnPoint(spawnPoint);
	}

	// === Boss Spawn
	propName = "BossSpawn";
	prop = data->GetProperty(propName);
	int id = atoi(prop->value.c_str());
	if (id > 0) {
		SetBossSpawnPoint((MinionSpawnPoint*)objectMap.find((unsigned int)id)->second);
	}

	// === Minion Waves
	m_vMinionWaves.clear();

	propName = "WaveCount";
	prop = data->GetProperty(propName);
	int waveCount = atoi(prop->value.c_str());
	SetNumberOfWaves(waveCount);
	for (int i = 0; i < waveCount; ++i) {
		propName = "MinionTypeCount" + std::to_string(i);
		prop = data->GetProperty(propName);
		int minionTypeCount = atoi(prop->value.c_str());
		for (int j = 0; j < minionTypeCount; ++j){
			propName = std::to_string(i) + "WaveMinion" + std::to_string(j);
			prop = data->GetProperty(propName);
			MinionType type = (MinionType)atoi(prop->value.c_str());
			AddMinionToWave(i, type);
		}
	}

	if (m_eTeam == RED) {
		propName = "ShieldDisableParticle";
		prop = data->GetProperty(propName);
		int ID = atoi(prop->value.c_str());
		if (ID > 0){
			m_pShieldDisableParticle = (ParticleSystem*)objectMap.find((unsigned int)ID)->second;
		}

		propName = "Shield";
		prop = data->GetProperty(propName);
		if (prop != nullptr) {
			ID = atoi(prop->value.c_str());
			if (ID > 0) {
				m_pShield = (GameObject*)objectMap.find((unsigned int)ID)->second;
			}
		}

		propName = "Vulnerable";
		prop = data->GetProperty(propName);
		if (std::strcmp(prop->value.c_str(), "False") == 0)
			m_fcMinionSpawnerFlags.SetFlag((unsigned int)MinionSpawnerFlags::MS_Vunerable_Flag, (unsigned int)false);
		else
			m_fcMinionSpawnerFlags.SetFlag((unsigned int)MinionSpawnerFlags::MS_Vunerable_Flag, (unsigned int)true);
	}

	propName = "SpawnDelay";
	prop = data->GetProperty(propName);
	m_fSpawnDelay = (float)atof(prop->value.c_str());

	propName = "WaveDelay";
	prop = data->GetProperty(propName);
	m_fWaveDelay = (float)atof(prop->value.c_str());

	propName = "BossFrequency";
	prop = data->GetProperty(propName);
	m_uiBossFrequency = (unsigned int)atoi(prop->value.c_str());

	propName = "Channel";
	prop = data->GetProperty(propName);
	m_iChannel = atoi(prop->value.c_str());

	propName = "SpawnMinions";
	prop = data->GetProperty(propName);
	if (std::strcmp(prop->value.c_str(),"False") == 0)
		m_fcMinionSpawnerFlags.SetFlag((unsigned int)MinionSpawnerFlags::MS_SpawnMinions_Flag, (unsigned int)false);
	else
		m_fcMinionSpawnerFlags.SetFlag((unsigned int)MinionSpawnerFlags::MS_SpawnMinions_Flag, (unsigned int)true);

	propName = "MainSpawner";
	prop = data->GetProperty(propName);
	if (std::strcmp(prop->value.c_str(), "False") == 0)
		m_fcMinionSpawnerFlags.SetFlag((unsigned int)MinionSpawnerFlags::MS_MainSpawner_Flag, (unsigned int)false);
	else
		m_fcMinionSpawnerFlags.SetFlag((unsigned int)MinionSpawnerFlags::MS_MainSpawner_Flag, (unsigned int)true);
}

void MinionSpawner::OnEnable()
{
	GameObject* foundObject = transform->GetChildByName(std::string("Health Bar"));
	if (foundObject != nullptr) {
		m_pHealthBar = foundObject->GetComponent<SpriteRenderer>();
		foundObject->SetActive(false, true);
	}

	BaseTower::OnEnable();

	// === Make sure to disable the Particle System, since it grabs the wrong one
	m_pDamageParticleSystem = nullptr;

	if (m_eTeam == Team::RED && m_pBossSpawn != nullptr) {
		// === Red Team, Handle the Boss Event
		if (s_pBossEvent == nullptr) {
			GameObject* BEObject = Inferno::Instantiate("BossEvent");
			ASSERT(BEObject->GetPrefabID() != 0, "No Boss Event Prefab was found");
			s_pBossEvent = BEObject->GetComponent<BossEvent>();
		}

		s_pBossEvent->RegisterSpawner(this);
		SetBossWave(MinionType::BOSS, m_uiBossFrequency);
	}
	
	// === Get the Alert Messager
	if (m_pAlertMessage == nullptr) {
		UIElement* foundEle = Inferno_UI::GetUIElementByName(std::string("Alert Message"), std::string("ExtendedHUD"));
		if (foundEle != nullptr) {
			m_pAlertMessage = foundEle->GetComponent<AlertMessage>();
		}
	}

	// === Deactivate Icon stuff
	if (m_fcMinionSpawnerFlags.GetFlag((unsigned int)MinionSpawnerFlags::MS_MainSpawner_Flag) && m_pDeactivatedIconSprite == nullptr) {
		GameObject* deactivateIconObject = Inferno::Instantiate("Deactivated Icon");
		deactivateIconObject->transform->SetParent(transform);
		deactivateIconObject->transform->SetPosition(0.0f, 2.5f, 0.0f);

		m_pDeactivatedIconSprite = deactivateIconObject->GetComponent<SpriteRenderer>();
		m_pDeactivatedIconSprite->SetRadialOrigin(SpriteRadialOrigins::RADIAL_ORIGIN_TOP);
		m_pDeactivatedIconSprite->SetFillAmount(0.0f);

		// === Turn off the renderer next frame until we are ready to use it
		deactivateIconObject->SetActive(false, true);
	}

	// === Control Point Stuff
	if (m_pShieldDisableParticle != nullptr) {
		m_pShieldDisableParticle->Pause(true);
	}

	EventSystem* pEventSystem = EventSystem::GetInstance();
	pEventSystem->RegisterForEvent("ControlPointCaptured", this, BindEventFunction(&MinionSpawner::ActivateOnEvent, this));
	pEventSystem->RegisterForEvent("UpgradeTowers", this, BindEventFunction(&MinionSpawner::OnUpgrade, this));

	if (m_fcMinionSpawnerFlags.GetFlag((unsigned int)MinionSpawnerFlags::MS_SpawnMinions_Flag)) {
		pEventSystem->RegisterForEvent("CutSceneActive", this, BindEventFunction(&MinionSpawner::OnCutSceneActive, this));
	}

	size_t count = m_vSpawnPoints.size();
	for (size_t i = 0; i < count; ++i) {
		m_vSpawnPoints[i]->gameObject->SetActive(false, true);
	}

	// === MiniMap Stuff
	CreateBossSpawnPing();
	CreateSpawnerUnderAttackPing();
}

void MinionSpawner::OnDisable()
{
	m_pAlertMessage = nullptr;

	EventSystem* pEventSystem = EventSystem::GetInstance();
	pEventSystem->UnregisterFromEvent("ControlPointCaptured", this, BindEventFunction(&MinionSpawner::ActivateOnEvent, this));
	pEventSystem->UnregisterFromEvent("CutSceneActive", this, BindEventFunction(&MinionSpawner::OnCutSceneActive, this));
	pEventSystem->UnregisterFromEvent("UpgradeTowers", this, BindEventFunction(&MinionSpawner::OnUpgrade, this));

	ClearMiniMapPingEvents();

	if (!m_itBossSpawnPingTimer.IsFinished()) {
		EndBossSpawnPing();
	}

	BaseTower::OnDisable();
}

void MinionSpawner::OnDestroy()
{
	m_pDeactivatedIconSprite = nullptr;

	m_pBossSpawn = nullptr;

	m_vOnActivationEvents.clear();

	m_pDeactivateTimer->SetTime(0.0f);
	m_pDeactivateTimer->Reset();

	m_uiCurrentWave = 0;
	m_uiMinionsSpawned = 0;

	m_fInitialSpawnDelay = INITIAL_SPAWN_DELAY;
}

void MinionSpawner::Update()
{
	if (CheatEntered(Cheat::SPAWN_FRIENDLY_WAVE) && m_eTeam == Team::BLUE) {
		SpawnWave();
		Inferno::PostAudioEventAtObjectPosition(DEFAULT_MINION_SPAWN, gameObject);
	}

	if (m_fInitialSpawnDelay > 0.0f) {
		m_fInitialSpawnDelay -= Time::DeltaTime;
	}

	if (gameObject->GetName() == "BlueMinionSpawner Test") {
		bool spawn = m_fcMinionSpawnerFlags.GetFlag((unsigned int)MinionSpawnerFlags::MS_SpawnMinions_Flag);
		int debug = 0;
	}

	if (m_fInitialSpawnDelay <= 0.0f && m_pSpawnTimer->IsFinished() && m_fcMinionSpawnerFlags.GetFlag((unsigned int)MinionSpawnerFlags::MS_SpawnMinions_Flag)) {
		if (gameObject->GetName() == "BlueMinionSpawner Test") {
			bool spawn = m_fcMinionSpawnerFlags.GetFlag((unsigned int)MinionSpawnerFlags::MS_SpawnMinions_Flag);
			int debug = 0;
		}

		SpawnWave();
		Inferno::PostAudioEventAtObjectPosition(DEFAULT_MINION_SPAWN, gameObject);
	}

	if (m_fcMinionSpawnerFlags.GetFlag((unsigned int)MinionSpawnerFlags::MS_Deactivated_Flag)) {
		m_pDeactivatedIconSprite->SetFillAmount((DEACTIVATED_TIME - m_pDeactivateTimer->TimeRemaining()) / DEACTIVATED_TIME);
	}

	UpdateBossSpawnPing();
	UpdateSpawnerUnderAttackPing();
}

void MinionSpawner::TakeDamage(DamageInfo _damageInfo)
{
 	if (!GetVulnerable() || IsDeactivated()) {
		// === Shield is up or it's deactivated, no damage
		return;
	}

	BaseTower::TakeDamage(_damageInfo);

	Inferno::PostAudioEventAtObjectPosition(DEFAULT_TURRET_TAKE_DAMAGE, gameObject);

	if (m_eTeam == Team::BLUE && m_pAlertMessage != nullptr) {
		Inferno::PostAudioEvent(MEGA_TANK_ALERT);
		m_pAlertMessage->SetFlashColors(Color(1.0f, 0.0f, 0.0f, 1.0f), Color(1.0f, 1.0f, 1.0f, 1.0f));
		m_pAlertMessage->DisplayMessage(std::string("Minion Spawner Under Attack!"), 1.5f, true);

		// === Ping Spawner Under Attack MiniMap Event
		StartSpawnerUnderAttackPing();
	}
}

BossMinion* MinionSpawner::SpawnBoss(MinionType _bossMinion)
{
	// === Stop all music, play the Boss Music
	Inferno::StopAllAudio();
	Inferno::PostAudioEvent(BOSS_EVENT_MUSIC);

	// === Spawn the Boss
	GameObject* boss = Inferno::Instantiate(GetNameFromType(_bossMinion));
	NavAgent* navAgent = boss->GetComponent<NavAgent>();
	if (m_pBossSpawn != nullptr) {
		boss->transform->SetPosition(m_pBossSpawn->transform->GetPosition());
		boss->transform->SetRotation(m_pBossSpawn->transform->WorldMatrix().GetRotation());
		navAgent->SetWaypoint(m_pBossSpawn->GetStartingWaypoint());
	}
	else {
		ASSERT(m_vSpawnPoints.size() > 0, "No Location to Spawn the Boss at");
		boss->transform->SetPosition(m_vSpawnPoints[0]->transform->GetPosition());
		boss->transform->SetRotation(m_vSpawnPoints[0]->transform->GetRotation());
		navAgent->SetWaypoint(m_vSpawnPoints[0]->GetStartingWaypoint());
	}

	// === Alert the Player
	if (m_pAlertMessage != nullptr) {
		m_pAlertMessage->QueueMessage("! Boss Incoming !", 2.0f, Color(1.0f, 0.0f, 0.0f, 1.0f), Color(1.0f, 1.0f, 1.0f, 1.0f), true, true);
	}

	// === Ping Boss Spawn MiniMap Event
	m_pTransformBossMinion = boss->transform;
	StartBossSpawnPing();

	return boss->GetComponent<BossMinion>();
}

void MinionSpawner::DeactivateShield()
{
	// === Disable shield gameobject
	if (m_pShield) {
		m_pShield->GetComponent<MeshRenderer>()->SetColor(Color(0.0f, 0.0f, 0.0f, 0.0f));
		m_pShield->GetComponent<SphereCollider>()->SetEnabled(false);
	}
	// === Start particle effect
	if (m_pShieldDisableParticle) {
		m_pShieldDisableParticle->Pause(false);
	}

	m_fcMinionSpawnerFlags.SetFlag((unsigned int)MinionSpawnerFlags::MS_Vunerable_Flag, (unsigned int)true);
	Inferno::PostAudioEvent(ABILITY_SHIELD_DOWN);
}

void MinionSpawner::AddForOnActivationStatus(void* _callingObject, OnActivationFunc _func)
{
	// === Make sure the requesting object isn't already tracking this Spawner
	size_t count = m_vOnActivationEvents.size();
	for (size_t i = 0; i < count; ++i) {
		if (m_vOnActivationEvents[i].CallingObject == _callingObject) {
#if (defined(_DEBUG))
			ASSERT(false, "Attempted to track the activation status of a Spawner by the same Object multiple times.");
#endif
			return;
		}
	}

	ActivationEvent newEvent = { _callingObject, _func };
	m_vOnActivationEvents.push_back(newEvent);
}

void MinionSpawner::RemoveFromOnActivationStatus(void* _callingObject)
{
	size_t count = m_vOnActivationEvents.size();
	for (size_t i = 0; i < count; ++i) {
		if (m_vOnActivationEvents[i].CallingObject == _callingObject) {
			m_vOnActivationEvents.erase(m_vOnActivationEvents.begin() + i);
			return;
		}
	}
}
// ===================== //

// === Protected Interface === //
IComponent& MinionSpawner::operator=(IComponent& _rhs)
{
	MinionSpawner* rhsMS = dynamic_cast<MinionSpawner*>(&_rhs);
	if (rhsMS == nullptr) {
		return *this;
	}

	return operator=(*rhsMS);
}

MinionSpawner& MinionSpawner::operator=(const MinionSpawner& _rhs)
{
	if (this != &_rhs) {
		*m_pSpawnTimer = *_rhs.m_pSpawnTimer;
		m_vMinionWaves = _rhs.m_vMinionWaves;
		m_fSpawnDelay = _rhs.m_fSpawnDelay;
		m_fWaveDelay = _rhs.m_fWaveDelay;
		m_pShield = _rhs.m_pShield;
		m_pShieldDisableParticle = _rhs.m_pShieldDisableParticle;
		m_fcMinionSpawnerFlags = _rhs.m_fcMinionSpawnerFlags;

		BaseTower::operator=(_rhs);
	}

	return *this;
}

IComponent* MinionSpawner::CreateCopy(GameObject* const gameObject, Transform* const transform)
{
	MinionSpawner* newMSpawner = new MinionSpawner(gameObject, transform);
	*newMSpawner = *this;
	return newMSpawner;
}
// =========================== //

// ===== Private Interface ===== //
void MinionSpawner::Death()
{
	// === Destroyed Particles
	GameObject* particlesObject = Inferno::GetNewGameObject();
	particlesObject->transform->SetPosition(transform->GetPosition() + Vector3(0, 1.0f, 0));
	ParticleSystem* part = particlesObject->AddComponent<ParticleSystem>();
	part->SetShape(EMITTER_SHAPE_SPHERE);
	part->SetStyle(EMITTER_STYLE_EDGE);
	part->SetSphereDimensions(0.1f);
	part->SetEmissionRate(200);
	part->SetDuration(0.3f);
	part->SetLifetime(0.4f);
	part->SetStartSpeed(7.0f);
	part->SetStartSize(0.1f);
	part->SetEndSize(0.01f);
	part->SetStartColor(Color(204.0f / 255.0f, 102.0f / 255.0f, 0.0f, 1.0f));
	part->SetEndColor(Color(204.0f / 255.0f, 102.0f / 255.0f, 0.0f, 1.0f));
	part->Play();
	part = particlesObject->AddComponent<ParticleSystem>();
	part->SetShape(EMITTER_SHAPE_SPHERE);
	part->SetStyle(EMITTER_STYLE_EDGE);
	part->SetSphereDimensions(0.1f);
	part->SetEmissionRate(200);
	part->SetDuration(0.3f);
	part->SetLifetime(0.4f);
	part->SetStartSpeed(7.0f);
	part->SetStartSize(0.2f);
	part->SetEndSize(0.03f);
	part->SetStartColor(Color(1.0f, 1.0f, 0.0f, 1.0f));
	part->SetEndColor(Color(1.0f, 1.0f, 0.0f, 1.0f));
	part = particlesObject->AddComponent<ParticleSystem>();
	part->SetShape(EMITTER_SHAPE_SPHERE);
	part->SetStyle(EMITTER_STYLE_EDGE);
	part->SetSphereDimensions(0.1f);
	part->SetEmissionRate(200);
	part->SetDuration(0.3f);
	part->SetLifetime(0.4f);
	part->SetStartSpeed(7.0f);
	part->SetStartSize(0.1f);
	part->SetEndSize(0.02f);
	part->SetStartColor(Color(1.0f, 0.0f, 0.0f, 1.0f));
	part->SetEndColor(Color(1.0f, 0.2f, 0.2f, 1.0f));
	Inferno::Destroy(particlesObject, part->GetDuration());

	// === Alert Message
	if (m_eTeam == Team::BLUE && m_pAlertMessage != nullptr) {
		m_pAlertMessage->QueueMessage("Minion Spawner Destroyed!", 2.5f, Color(0.75f, 0.0f, 0.0f, 1.0f), Color(0.85f, 0.85f, 0.85f, 1.0f), true, true);
	}

	if (m_fcMinionSpawnerFlags.GetFlag((unsigned int)MinionSpawnerFlags::MS_MainSpawner_Flag)) {
		// === Main Spawner, deactivate
		m_fcMinionSpawnerFlags.SetFlag((unsigned int)MinionSpawnerFlags::MS_Deactivated_Flag, (unsigned int)true);
		m_pDeactivateTimer->SetTime(DEACTIVATED_TIME);
		m_pDeactivateTimer->Reset();

		m_pSpawnTimer->SetTime(m_fWaveDelay * DEACTIVATED_SPAWNDELAY_MULTIPLIER);
		m_pSpawnTimer->Reset();

		m_pHealthBar->gameObject->SetActive(false);

		m_pDeactivatedIconSprite->gameObject->SetActive(true);

		// === Alert trackers that it's deactivating
		size_t count = m_vOnActivationEvents.size();
		for (size_t i = 0; i < count; ++i) {
			m_vOnActivationEvents[i].Func(false);
		}
	}
	else {
		// === Not a Main Spawner, just destroy as normal
		BaseTower::Death();
	}
}

void MinionSpawner::SpawnWave()
{
	if (m_vMinionWaves.empty()) {
		return;
	}

	size_t count;

	// === Handle Playing the Particle Effects (First Stage)
	if (!m_vSpawnPoints[0]->gameObject->IsActive()) {
		// === Turn on the Particle Emitters
		count = m_vSpawnPoints.size();
		for (size_t i = 0; i < count; ++i) {
			m_vSpawnPoints[i]->gameObject->SetActive(true);
		}

		m_pSpawnTimer->SetTime(0.5f);
		return;
	}

	// === Determine how many Minions to spawn
	unsigned int minionSpawnCount = (m_fcMinionSpawnerFlags.GetFlag((unsigned int)MinionSpawnerFlags::MS_Deactivated_Flag) ? DEACTIVATED_SPAWNCOUNT : (unsigned int)m_vMinionWaves[m_uiCurrentWave].size());

	// === Spawn the Wave (Second Stage)
	float newHealth;
	GameObject* minion;

	count = m_vSpawnPoints.size();
	for (size_t i = 0; i < count; ++i) {
		minion = Inferno::Instantiate(GetNameFromType(m_vMinionWaves[m_uiCurrentWave][m_uiMinionsSpawned]));
		minion->transform->SetPosition(m_vSpawnPoints[i]->transform->GetPosition());
		minion->transform->SetRotation(m_vSpawnPoints[i]->transform->WorldMatrix().GetRotation());

		NavAgent* navAgent = minion->GetComponent<NavAgent>();
		if (navAgent == nullptr) {
			Inferno::Destroy(minion);
			PRINT("A Minion was missing a NavAgent.");
			continue;
		}
		navAgent->SetWaypoint(m_vSpawnPoints[i]->GetStartingWaypoint());

		if (m_eTeam == Team::RED && m_uiUpgradeLevel > 0) {
			Entity* entity = minion->GetComponent<Entity>();
			newHealth = entity->GetMaxHealth() * (1 + (m_uiUpgradeLevel * MINION_HEALTH_MULTIPLIER));
			entity->SetMaxHealth(newHealth);
			entity->SetHealth(newHealth);
		}
		
		if (++m_uiMinionsSpawned >= minionSpawnCount) {
			// === Done spawning this wave, reset and start the wave delay timer
			m_uiMinionsSpawned = 0;
			m_uiCurrentWave = (m_uiCurrentWave >= m_vMinionWaves.size() - 1 ? 0 : m_uiCurrentWave + 1);
			m_pSpawnTimer->SetTime(m_fWaveDelay);

			if (m_eTeam == Team::RED && s_pBossEvent != nullptr) {
				s_pBossEvent->AlertWaveSpawned();
			}

			// === Turn off the Particle Emitters (Final Stage)
			count = m_vSpawnPoints.size();
			for (size_t i = 0; i < count; ++i) {
				m_vSpawnPoints[i]->gameObject->SetActive(false);
			}
			return;
		}
	}

	// === Not Done Spawning
	m_pSpawnTimer->SetTime(m_fSpawnDelay);
}

std::string MinionSpawner::GetNameFromType(MinionType _type)
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

	if (m_eTeam == Team::BLUE) {
		return (typeName += "Blue");
	}
	else {
		return (typeName += "Red");
	}
}

void MinionSpawner::UpgradePlayerSpawner()
{
	++m_uiUpgradeLevel;

	if (m_uiUpgradeLevel == 1) {
		// === Add on Two more Minions
		AddMinionToWave(0, MinionType::MINI);
		AddMinionToWave(0, MinionType::MINI);
	}
	else if (m_uiUpgradeLevel == 2) {
		// === Add a new Wave, with Tanks
		SetNumberOfWaves(2);

		AddMinionToWave(1, MinionType::MINI);
		AddMinionToWave(1, MinionType::MINI);
		AddMinionToWave(1, MinionType::TANK);
		AddMinionToWave(1, MinionType::TANK);
	}
}

// === CreateBossSpawnPing
//  IN : void   
//  OUT : void
//
//  Creates and Registers Boss Spawn MiniMap Ping Event
void MinionSpawner::CreateBossSpawnPing()
{
	// Set Up Boss Spawn Event MiniMap Ping
	if (m_pElementBossSpawnPing == nullptr)
	{
		// Create Element
		m_pElementBossSpawnPing = new UIElement();
		m_pElementBossSpawnPing->SetName("BossSpawnPingIcon");

		// Set Element Transform Data
		m_pTransformBossSpawnPing = m_pElementBossSpawnPing->GetTransform();
		m_pTransformBossSpawnPing->SetAnchors(Anchors::ANCHOR_TOPLEFT);
		m_pTransformBossSpawnPing->SetBounds(Vector2(m_fMaxPingSize, m_fMaxPingSize));

		// Attach Renderer Component And Set Its Data
		m_pRendererBossSpawnPing = m_pElementBossSpawnPing->AddComponent<UIRenderer>();
		m_pRendererBossSpawnPing->SetTexture(m_pIconBossSpawnPing);
		m_pRendererBossSpawnPing->SetColor(Color(1.0f, 0.0f, 0.0f, 1.0f));
		m_pRendererBossSpawnPing->SetLayer(4);
		m_pRendererBossSpawnPing->SetEnabled(false); // default to false, true when active
	}

	// If The Icon Boss Spawn Renderer Is Already Set, Make Sure It Isn't Enabled
	if (m_pRendererBossSpawnPing)
		m_pRendererBossSpawnPing->SetEnabled(false);
}

// === CreateSpawnerUnderAttackPing
//  IN : void  
//  OUT : void
//
//  Creates and Registers Spawner Under Attack MiniMap Ping Event
void MinionSpawner::CreateSpawnerUnderAttackPing()
{
	// Set Up Spawner Under Attack MiniMap Ping
	if (m_pElementUnderAttackPing == nullptr)
	{
		// Create Element
		m_pElementUnderAttackPing = new UIElement();
		m_pElementUnderAttackPing->SetName("SpawnerUnderAttackPingIcon");

		// Set Element Transform Data
		m_pTransformUnderAttackPing = m_pElementUnderAttackPing->GetTransform();
		m_pTransformUnderAttackPing->SetAnchors(Anchors::ANCHOR_TOPLEFT);
		m_pTransformUnderAttackPing->SetBounds(Vector2(m_fMaxPingSize, m_fMaxPingSize));

		// Attach Renderer Component And Set Its Data
		m_pRendererUnderAttackPing = m_pElementUnderAttackPing->AddComponent<UIRenderer>();
		m_pRendererUnderAttackPing->SetTexture(m_pIconUnderAttackPing);
		m_pRendererUnderAttackPing->SetColor(Color(1.0f, 0.0f, 0.0f, 1.0f));
		m_pRendererUnderAttackPing->SetLayer(4);
		m_pRendererUnderAttackPing->SetEnabled(false); // default to false, true when active

		// Set Up MiniMap Element
		m_sMapElementUnderAttackPing.element = m_pElementUnderAttackPing;
		m_sMapElementUnderAttackPing.transform = this->transform;
	}

	// If The Icon Boss Spawn Renderer Is Already Set, Make Sure It Isn't Enabled
	if (m_pRendererUnderAttackPing)
		m_pRendererUnderAttackPing->SetEnabled(false);

	// Register Boss Spawn Event With MiniMap
	EventSystem::GetInstance()->SendEventNow("EntityInstantiate", &m_sMapElementUnderAttackPing);
}

// === ClearMiniMapPingEvents
//  IN : void 
//  OUT : void
//
//  Clears MiniMap Ping Renderers And Unregisters From The MiniMap
void MinionSpawner::ClearMiniMapPingEvents()
{
	if (m_pRendererBossSpawnPing)
		m_pRendererBossSpawnPing->SetEnabled(false);

	if (m_pRendererUnderAttackPing)
		m_pRendererUnderAttackPing->SetEnabled(false);

	EventSystem::GetInstance()->SendEventNow("EntityDestroy", &m_sMapElementUnderAttackPing);
}

// === UpdateBossSpawnPing
//  IN : void 
//  OUT : void
//
//  Updates Boss Spawn Ping If Active
void MinionSpawner::UpdateBossSpawnPing()
{
	if (m_itBossSpawnPingTimer.IsFinished() == true)
		return;

	float lerpRatio = m_itBossSpawnPingTimer.TimeRemaining() / m_itBossSpawnPingTimer.GetSetTimeSeconds();
	float lerpValue = (m_fMinPingSize * (1.0f - lerpRatio) + (m_fMaxPingSize * lerpRatio));

	m_pTransformBossSpawnPing->SetBounds(Vector2(lerpValue, lerpValue));
}

// === UpdateSpawnerUnderAttackPing
//  IN : void
//  OUT : void
//
//  Updates Spawner Under Attack Ping If Active
void MinionSpawner::UpdateSpawnerUnderAttackPing()
{
	if (m_itUnderAttackPingTimer.IsFinished() == true)
		return;

	float lerpRatio = m_itUnderAttackPingTimer.TimeRemaining() / m_itUnderAttackPingTimer.GetSetTimeSeconds();
	float lerpValue = (m_fMinPingSize * (1.0f - lerpRatio) + (m_fMaxPingSize * lerpRatio));

	m_pTransformUnderAttackPing->SetBounds(Vector2(lerpValue, lerpValue));
}

// === StartBossSpawnPing
//  IN : void
//  OUT : void
//
//  Starts The Boss Spawn MiniMap Ping Event
void MinionSpawner::StartBossSpawnPing()
{
	if (m_fcMinionSpawnerFlags.GetFlag((unsigned int)MinionSpawnerFlags::MS_BossSpawnPing_Flag))
	{
		m_fcMinionSpawnerFlags.SetFlag((unsigned int)MinionSpawnerFlags::MS_BossSpawnPing_Flag, (unsigned int)false);
		m_pRendererBossSpawnPing->SetEnabled(true);
		m_itBossSpawnPingTimer.SetTime(m_fPingTime);
		m_itBossSpawnPingTimer.SetOnTimerEndFunction(BindOnTimerEndFunction(&MinionSpawner::EndBossSpawnPing, this));

		// Set Up MiniMap Element
		m_sMapElementBossSpawnPing.element = m_pElementBossSpawnPing;
		m_sMapElementBossSpawnPing.transform = m_pTransformBossMinion;

		// Register Boss Spawn Event With MiniMap
		EventSystem::GetInstance()->SendEventNow("EntityInstantiate", &m_sMapElementBossSpawnPing);

		bool pData;
		EventSystem::GetInstance()->SendEventNow("WarningEvent", &pData);
	}
}

// === EndBossSpawnPing
//  IN : void
//  OUT : void
//
//  Ends The Boss Spawn MiniMap Ping Event
void MinionSpawner::EndBossSpawnPing()
{
	m_fcMinionSpawnerFlags.SetFlag((unsigned int)MinionSpawnerFlags::MS_BossSpawnPing_Flag, (unsigned int)true);
	m_pRendererBossSpawnPing->SetEnabled(false);
	m_pTransformBossSpawnPing->SetBounds(Vector2(m_fMaxPingSize, m_fMaxPingSize));
	m_pTransformBossMinion = nullptr;

	EventSystem::GetInstance()->SendEventNow("EntityDestroy", &m_sMapElementBossSpawnPing);
}

// === StartSpawnerUnderAttackPing
//  IN : void
//  OUT : void
//
//  Starts The Spawner Under Attack MiniMap Ping Event
void MinionSpawner::StartSpawnerUnderAttackPing()
{
	if (m_fcMinionSpawnerFlags.GetFlag((unsigned int)MinionSpawnerFlags::MS_UnderAttackPing_Flag))
	{
		m_fcMinionSpawnerFlags.SetFlag((unsigned int)MinionSpawnerFlags::MS_UnderAttackPing_Flag, (unsigned int)false);
		m_pRendererUnderAttackPing->SetEnabled(true);
		m_itUnderAttackPingTimer.SetTime(m_fPingTime);
		m_itUnderAttackPingTimer.SetOnTimerEndFunction(BindOnTimerEndFunction(&MinionSpawner::EndSpawnerUnderAttackPing, this));

		bool pData;
		EventSystem::GetInstance()->SendEventNow("WarningEvent", &pData);
	}
}

// === EndSpawnerUnderAttackPing
//  IN : void
//  OUT : void
//
//  Ends The Spawner Under Attack MiniMap Ping Event
void MinionSpawner::EndSpawnerUnderAttackPing()
{
	m_fcMinionSpawnerFlags.SetFlag((unsigned int)MinionSpawnerFlags::MS_UnderAttackPing_Flag, (unsigned int)true);
	m_pRendererUnderAttackPing->SetEnabled(false);
	m_pTransformUnderAttackPing->SetBounds(Vector2(m_fMaxPingSize, m_fMaxPingSize));
}
// ============================= //

// === Events === //

// === ActivateOnEvent
//  IN : void* - What channel the barrier event is on
//
//  OUT : void
//
//  Activates the spawner if its on the right channel
void MinionSpawner::ActivateOnEvent(void * _data)
{
	int _channel = (int)_data;

	if (_channel == m_iChannel) {
		m_fcMinionSpawnerFlags.SetFlag((unsigned int)MinionSpawnerFlags::MS_SpawnMinions_Flag, (unsigned int)true);
		EventSystem::GetInstance()->RegisterForEvent("CutSceneActive", this, BindEventFunction(&MinionSpawner::OnCutSceneActive, this));
	}

	if (m_fcMinionSpawnerFlags.GetFlag((unsigned int)MinionSpawnerFlags::MS_Vunerable_Flag)) {
		DeactivateShield();
	}
}

void MinionSpawner::OnActivate()
{
	m_fcMinionSpawnerFlags.SetFlag((unsigned int)MinionSpawnerFlags::MS_Deactivated_Flag, (unsigned int)false);
	m_pDeactivateTimer->SetTime(0.0f);
	m_pDeactivateTimer->Reset();

	m_pDeactivatedIconSprite->gameObject->SetActive(false);

	m_fHealth = m_fMaxHealth;

	size_t count = m_vOnActivationEvents.size();
	for (size_t i = 0; i < count; ++i) {
		m_vOnActivationEvents[i].Func(true);
	}
}

void MinionSpawner::OnCutSceneActive(void* _pData)
{
	bool spawnMinions = !(*(static_cast<bool*>(_pData)));
	m_fcMinionSpawnerFlags.SetFlag((unsigned int)MinionSpawnerFlags::MS_SpawnMinions_Flag, (unsigned int)spawnMinions);
}

void MinionSpawner::OnUpgrade(void* _pData)
{
	Team targetTeam = (Team)*(static_cast<unsigned int*>(_pData));

	if (targetTeam != m_eTeam) {
		return;
	}

	if (targetTeam == Team::RED) {
		++m_uiUpgradeLevel;

		EventSystem::GetInstance()->SendEventNow("ToolTip_SpawnerUpgrade", nullptr);

		// === Upgrade Effect
		GameObject* effect = Inferno::Instantiate("TowerUpgradeEffect");
		Vector3 pos = transform->GetPosition();
		pos.y += 0.15f;
		effect->transform->SetPosition(pos);
	}
	else {
		TowerTypes targetType = (TowerTypes)*(static_cast<unsigned int*>(_pData)+1);
		if (targetType == TowerTypes::TT_SPAWNER) {
			UpgradePlayerSpawner();

			// === Upgrade Effect
			GameObject* effect = Inferno::Instantiate("TowerUpgradeEffect");
			Vector3 pos = transform->GetPosition();
			pos.y += 0.15f;
			effect->transform->SetPosition(pos);
		}
	}
}
// ============== //

// ===== Accessors ===== // 
bool MinionSpawner::GetVulnerable()
{
	return m_fcMinionSpawnerFlags.GetFlag((unsigned int)MinionSpawnerFlags::MS_Vunerable_Flag);
}

bool MinionSpawner::IsDeactivated()
{
	return m_fcMinionSpawnerFlags.GetFlag((unsigned int)MinionSpawnerFlags::MS_Deactivated_Flag);
}
// ===================== //

// ===== Mutators ===== //
void MinionSpawner::AddSpawnPoint(MinionSpawnPoint* _spawnPoint)
{
	m_vSpawnPoints.push_back(_spawnPoint);
}

void MinionSpawner::AddMinionToWave(unsigned int _wave, MinionType _type)
{
	if (_wave >= m_vMinionWaves.size()) {
		return;
	}

	m_vMinionWaves[_wave].push_back(_type);
}

void MinionSpawner::SetBossSpawnPoint(MinionSpawnPoint* _spawnPoint)
{
	m_pBossSpawn = _spawnPoint;
}

void MinionSpawner::SetMinionType(unsigned int _wave, unsigned int _minionSlot, MinionType _newType)
{
	if (_wave >= m_vMinionWaves.size() || _minionSlot >= m_vMinionWaves[_wave].size()) {
		return;
	}

	m_vMinionWaves[_wave][_minionSlot] = _newType;
}

void MinionSpawner::SetNumberOfWaves(unsigned int _numWaves)
{
	m_vMinionWaves.resize(_numWaves);
}

void MinionSpawner::SetEventChannel(int _channel)
{
	m_iChannel = _channel;
}

void MinionSpawner::SetIsActivated(bool activated)
{
	m_fcMinionSpawnerFlags.SetFlag((unsigned int)MinionSpawnerFlags::MS_SpawnMinions_Flag, activated ? 1 : 0);
}

// === SetSpawnDelay
//  IN : float - The delay in seconds
//  OUT : void
//
//  Sets the delay for minions spawning.
void MinionSpawner::SetSpawnDelay(float _delay)
{
	m_fSpawnDelay = _delay;
}

// === SetWaveDelay
//  IN : float - The delay in seconds
//  OUT : void
//
//  Sets the delay between waves starting to spawn.
void MinionSpawner::SetWaveDelay(float _delay)
{
	m_fWaveDelay = _delay;
}

// === SetBossWave
//  IN : MinionType - Type of Minion to spawn during the Boss Wave
//       unsigned int - How frequent the Boss Wave will spawn, in number of waves.
//  OUT : void
//
//  Sets up the Information for the Boss Wave, what should spawn and when. 
void MinionSpawner::SetBossWave(MinionType _boss, unsigned int _frequency)
{
	if (s_pBossEvent == nullptr) {
		GameObject* BEObject = Inferno::Instantiate("BossEvent");
		ASSERT(BEObject->GetPrefabID() != 0, "No Boss Event Prefab was found");
		s_pBossEvent = BEObject->GetComponent<BossEvent>();
	}

	s_pBossEvent->SetBossWave(_boss, _frequency);
}
// ==================== //