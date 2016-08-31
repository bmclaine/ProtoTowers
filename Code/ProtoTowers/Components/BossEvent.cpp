#include "stdafx.h"
#include "BossEvent.h"

#include "../InfernoUI.h"
#include "BossInfo.h"
#include "BossMinion.h"
#include "CutScene.h"
#include "EventSystem.h"
#include "MinionSpawner.h"

// === Defines
#define BOSS_HEALTH_INCREASE 200.0f

// === Constructor / Destructor === //
BossEvent::BossEvent(GameObject* _owner, Transform* _transform) : IComponent(_owner, _transform)
{
	m_bWaveSpawned = false;
	m_pBossInfo = nullptr;
	m_pBoss = nullptr;
	m_pBossCutScene = nullptr;
	m_pCutSceneSpawn = nullptr;
	m_uiBossSpawner = 0;
	m_uiBossCount = 0;
}

BossEvent::~BossEvent()
{

}
// ================================ //

// === Interface === //
void BossEvent::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{
	ObjectData* data = dataMap.find(GetInstanceID())->second;

	Property* prop = data->GetProperty(std::string("CutScene"));
	int id = -1;
	if (prop)
	{
		id = atoi(prop->value.c_str());
		if (id > 0)
		{
			m_pBossCutScene = (CutScene*)(objectMap.find((unsigned int)id)->second);
		}
	}

	prop = data->GetProperty(std::string("StartLocation"));
	if (prop)
	{
		id = atoi(prop->value.c_str());
		if (id > 0)
		{
			m_pCutSceneSpawn = (Transform*)(objectMap.find((unsigned int)id)->second);
		}
	}
}

void BossEvent::OnEnable()
{
	if (m_pBossInfo == nullptr) {
		CreateBossInfo();
	}

	m_uiInitialWaveDelay = 0;

	MinionSpawner::s_pBossEvent = this;
}

void BossEvent::OnDisable()
{
	if (m_pBoss != nullptr) {
		m_pBoss->RemoveFromTracking(this);
		m_pBoss->SetOnTakeDamageFunction(nullptr);
		m_pBoss = nullptr;
	}

	m_pBossInfo = nullptr;

	size_t count = m_vSpawners.size();
	for (size_t i = 0; i < count; ++i) {
		m_vSpawners[i]->RemoveFromTracking(this);
	}
	m_vSpawners.clear();

	MinionSpawner::s_pBossEvent = nullptr;
}

void BossEvent::Update()
{
	if (m_bWaveSpawned) {
		m_bWaveSpawned = false;
		if (m_uiInitialWaveDelay > 0) {
			--m_uiInitialWaveDelay;
		}
		else {
			--m_iWaveCounter;
		}

		// === Is it Time to Spawn the Boss?
		if (m_iWaveCounter <= 0 && m_pBoss == nullptr) {
			m_iWaveCounter = m_uiFrequency;
			m_pBoss = m_vSpawners[m_uiBossSpawner]->SpawnBoss(m_eBossType);
			m_pBoss->AddForTracking(this, BindEntityOnReleaseFunction(&BossEvent::OnBossKilled, this));

			// === Increase Boss Health
			float newHealth = m_pBoss->GetMaxHealth() + BOSS_HEALTH_INCREASE * m_uiBossCount;
			m_pBoss->SetMaxHealth(newHealth);
			m_pBoss->SetHealth(newHealth);

			if (m_uiBossCount == 0) {
				StartBossSpawnCutScene();
			}
			else if (m_uiBossCount == 1) {
				EventSystem::GetInstance()->SendEventNow("ToolTip_BossHealth", nullptr);
			}

			if (m_pBossInfo != nullptr) {
				m_pBossInfo->GetUIElement()->SetActive(true);
				m_pBoss->SetOnTakeDamageFunction(std::bind(&BossInfo::UpdateHealthBar, m_pBossInfo, std::placeholders::_1));
			}

			if (m_uiBossSpawner == m_vSpawners.size() - 1) {
				m_uiBossSpawner = 0;
			}
			else {
				++m_uiBossSpawner;
			}

			++m_uiBossCount;
		}
	}
}

void BossEvent::RegisterSpawner(MinionSpawner* _spawner)
{
	m_vSpawners.push_back(_spawner);
	_spawner->AddForTracking(this, BindEntityOnReleaseFunction(&BossEvent::OnSpawnerDestroyed, this));
}

void BossEvent::AlertWaveSpawned()
{
	if (m_vSpawners.size() > 0) {
		m_bWaveSpawned = true;
	}
}

void BossEvent::StartBossSpawnCutScene()
{
	Matrix4 bossSpawn = m_pBoss->transform->WorldMatrix();
	m_pCutSceneSpawn->SetWorldMatrix(bossSpawn);
	Vector3 rot = m_pCutSceneSpawn->transform->GetRotation();
	rot.y += IE_PI;
	m_pCutSceneSpawn->transform->SetRotation(rot);
	m_pBossInfo->GetUIElement()->SetActive(false);
	m_pBossCutScene->SetOnCutSceneEndFunction(std::bind(&BossEvent::EndBossSpawnCutScene, this));
	m_pBossCutScene->StartCutScene();
}

void BossEvent::EndBossSpawnCutScene()
{
	m_pBossInfo->GetUIElement()->SetActive(true);
}
// ================= //

// === Protected Interface === //
IComponent& BossEvent::operator=(IComponent& _rhs)
{
	BossEvent* rhsBE = dynamic_cast<BossEvent*>(&_rhs);
	if (rhsBE == nullptr) {
		return *this;
	}

	return operator=(*rhsBE);
}

BossEvent& BossEvent::operator=(const BossEvent& _rhs)
{
	if (this != &_rhs) {
		m_bWaveSpawned = _rhs.m_bWaveSpawned;
		m_eBossType = _rhs.m_eBossType;
		m_fcFlags = _rhs.m_fcFlags;
		m_uiBossSpawner = _rhs.m_uiBossSpawner;
		m_uiFrequency = _rhs.m_uiFrequency;
		m_uiBossCount = _rhs.m_uiBossCount;
		m_iWaveCounter = _rhs.m_iWaveCounter;
		//		m_vSpawners = _rhs.m_vSpawners;		// <-- Don't see any need for this, however incase something isn't working properly, don't forget about this
	}
	return *this;
}

IComponent* BossEvent::CreateCopy(GameObject* const gameObject, Transform* const transform)
{
	BossEvent* newBossEvent = new BossEvent(gameObject, transform);
	*newBossEvent = *this;

	return newBossEvent;
}
// =========================== //

// ===== Private Interface ===== //
void BossEvent::CreateBossInfo()
{
	using namespace Inferno_UI;

	UICanvas* hudCanvas = nullptr;
	unsigned int hudCanvasSlot;

	hudCanvas = GetCanvasByName("ExtendedHUD");
	if (hudCanvas == nullptr) {
		CreateNewCanvas("ExtendedHUD", &hudCanvas, &hudCanvasSlot);
	}

	UIElement* bossHPBar = new UIElement();
	bossHPBar->SetName("Boss HP Bar");
	m_pBossInfo = bossHPBar->AddComponent<BossInfo>();

	AddToView(bossHPBar, "ExtendedHUD");

	m_pBossInfo->GetUIElement()->SetActive(false);
}
// ============================= //

// ===== Events ===== //
void BossEvent::OnBossKilled(Entity* _boss)
{
	Inferno::StopAllAudio();
	Inferno::PostAudioEvent(GAME_BACKGROUND_MUSIC);
	m_pBoss = nullptr;
	m_iWaveCounter = m_uiFrequency;

	if (m_pBossInfo != nullptr) {
		m_pBossInfo->GetUIElement()->SetActive(false);
	}
}

void BossEvent::OnSpawnerDestroyed(Entity* _spawner)
{
	MinionSpawner* destroyedSpawner = (MinionSpawner*)_spawner;

	if (m_uiBossSpawner == m_vSpawners.size() - 1) {
		m_uiBossSpawner = 0;
	}

	size_t count = m_vSpawners.size();
	for (size_t i = 0; i < count; ++i) {
		if (m_vSpawners[i] == destroyedSpawner) {
			m_vSpawners.erase(m_vSpawners.begin() + i);
			break;
		}
	}
}
// ================== //

// ===== Mutators ===== //
void BossEvent::SetBossWave(MinionType _bossType, unsigned int _frequency)
{
	m_eBossType = _bossType;
	m_uiFrequency = m_iWaveCounter = _frequency;
}
// ==================== //