// Author : Jack Gillette
//
// Spawner Tower that handles spawning minions in waves
#pragma once
#include "BaseTower.h"

#include "BossEvent.h"

#include <vector>
#include "InfernoTimer.h"

class AlertMessage;
class BossMinion;
class InfernoTimer;
class MinionSpawnPoint;
class UIElement;
class UIRenderer;
class RectTransform;
class SpriteRenderer;
class Transform;
class Texture;

enum MinionType;

// === Defines
#define BindOnActivationStatusFunction(Function, Class) { std::bind(Function, Class, std::placeholders::_1) }

class MinionSpawner : public BaseTower
{
private:
	// === Enumerations
	enum class MinionSpawnerFlags { MS_SpawnMinions_Flag = 0, MS_Deactivated_Flag = 1, MS_MainSpawner_Flag = 2, MS_BossSpawnPing_Flag = 3, MS_UnderAttackPing_Flag = 4, MS_Vunerable_Flag = 5, MS_MAX_FLAGS = 6 };

	// === TypeDefs
	typedef std::function<void(bool)> OnActivationFunc;

	// === Structures
	struct MapElement {
		Transform* transform;
		UIElement* element;
	};

	struct ActivationEvent {
		void* CallingObject;
		OnActivationFunc Func;
	};

	// === Static Variables
	static BossEvent* s_pBossEvent;

	// === Variables
	std::vector<MinionSpawnPoint*> m_vSpawnPoints;
	std::vector<std::vector<MinionType>> m_vMinionWaves;
	std::vector<ActivationEvent> m_vOnActivationEvents;
	AlertMessage* m_pAlertMessage;
	InfernoTimer* m_pSpawnTimer;
	InfernoTimer* m_pDeactivateTimer;
	MinionSpawnPoint* m_pBossSpawn;
	ParticleSystem * m_pShieldDisableParticle;
	GameObject * m_pShield;
	RectTransform* m_pTransformBossSpawnPing;
	RectTransform* m_pTransformUnderAttackPing;
	SpriteRenderer* m_pDeactivatedIconSprite;
	Texture* m_pIconBossSpawnPing;
	Texture* m_pIconUnderAttackPing;
	Transform* m_pTransformBossMinion;
	UIElement* m_pElementBossSpawnPing;
	UIElement* m_pElementUnderAttackPing;
	UIRenderer* m_pRendererBossSpawnPing;
	UIRenderer* m_pRendererUnderAttackPing;
	Flags<char> m_fcMinionSpawnerFlags;
	InfernoTimer m_itBossSpawnPingTimer;
	InfernoTimer m_itUnderAttackPingTimer;
	MapElement m_sMapElementBossSpawnPing;
	MapElement m_sMapElementUnderAttackPing;
	int m_iChannel;
	unsigned int m_uiCurrentWave;
	unsigned int m_uiMinionsSpawned;
	unsigned int m_uiBossFrequency;
	float m_fSpawnDelay;
	float m_fWaveDelay;
	float m_fMaxPingSize;
	float m_fMinPingSize;
	float m_fPingTime;
	float m_fInitialSpawnDelay;

	// === Private Interface === //
	virtual void Death() override;
	void SpawnWave();
	std::string GetNameFromType(MinionType _type);
	void UpgradePlayerSpawner();
	// === CreateBossSpawnPing
	//  IN : void  
	//  OUT : void
	//
	//  Creates and Registers Boss Spawn MiniMap Ping Event
	void CreateBossSpawnPing();
	// === CreateSpawnerUnderAttackPing
	//  IN : void
	//  OUT : void
	//
	//  Creates and Registers Spawner Under Attack MiniMap Ping Event
	void CreateSpawnerUnderAttackPing();
	// === ClearMiniMapPingEvents
	//  IN : void
	//  OUT : void
	//
	//  Clears MiniMap Ping Renderers
	void ClearMiniMapPingEvents();
	// === UpdateBossSpawnPing
	//  IN : void
	//  OUT : void
	//
	//  Updates Boss Spawn Ping If Active
	void UpdateBossSpawnPing();
	// === UpdateSpawnerUnderAttackPing
	//  IN : void
	//  OUT : void
	//
	//  Updates Spawner Under Attack Ping If Active
	void UpdateSpawnerUnderAttackPing();
	// === StartBossSpawnPing
	//  IN : void
	//  OUT : void
	//
	//  Starts The Boss Spawn MiniMap Ping Event
	void StartBossSpawnPing();
	// === EndBossSpawnPing
	//  IN : void
	//  OUT : void
	//
	//  Ends The Boss Spawn MiniMap Ping Event
	void EndBossSpawnPing();
	// === StartSpawnerUnderAttackPing
	//  IN : void
	//  OUT : void
	//
	//  Starts The Spawner Under Attack MiniMap Ping Event
	void StartSpawnerUnderAttackPing();
	// === StartSpawnerUnderAttackPing
	//  IN : void
	//  OUT : void
	//
	//  Ends The Spawner Under Attack MiniMap Ping Event
	void EndSpawnerUnderAttackPing();
	// ========================= //

	// === Events === //

	// === ActivateOnEvent
	//  IN : void* - What channel the barrier event is on
	//
	//  OUT : void
	//
	//  Activates the spawner if its on the right channel
	void ActivateOnEvent(void * _data);
	void OnActivate();
	void OnCutSceneActive(void* _pData);
	void OnUpgrade(void* _pData);
	// ============== //

protected:
	// === Protected Interface === //
	virtual IComponent& operator=(IComponent& _rhs) override;
	MinionSpawner& operator=(const MinionSpawner& _rhs);
	virtual IComponent* CreateCopy(GameObject* const gameObject, Transform* const transform) override;
	// =========================== //

public:
	// === Constructor / Destructor / Operators === //
	MinionSpawner(GameObject* _owner, Transform* _transform);
	virtual ~MinionSpawner();
	// ================================ //

	// === Interface === //
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap);
	virtual void OnEnable() override;
	virtual void OnDisable() override;
	virtual void OnDestroy() override;
	virtual void Update() override;
	virtual void TakeDamage(DamageInfo _damageInfo) override;
	BossMinion* SpawnBoss(MinionType _bossMinion);
	void DeactivateShield();

	void AddForOnActivationStatus(void* _callingObject, OnActivationFunc _func);
	void RemoveFromOnActivationStatus(void* _callingObject);
	// ================= //

	// === Accessors === // 
	bool GetVulnerable();
	bool IsDeactivated();
	// ================= //

	// === Mutators === //
	void AddSpawnPoint(MinionSpawnPoint* _spawnPoint);
	void AddMinionToWave(unsigned int _wave, MinionType _type);
	void SetBossSpawnPoint(MinionSpawnPoint* _spawnPoint);
	void SetMinionType(unsigned int _wave, unsigned int _minionSlot, MinionType _newType);
	void SetNumberOfWaves(unsigned int _numWaves);
	void SetEventChannel(int _channel);
	void SetIsActivated(bool activated);
	// === SetSpawnDelay
	//  IN : float - The delay in seconds
	//  OUT : void
	//
	//  Sets the delay for minions spawning.
	void SetSpawnDelay(float _delay);
	// === SetWaveDelay
	//  IN : float - The delay in seconds
	//  OUT : void
	//
	//  Sets the delay between waves starting to spawn.
	void SetWaveDelay(float _delay);
	// === SetBossWave
	//  IN : MinionType - Type of Minion to spawn during the Boss Wave
	//       unsigned int - How frequent the Boss Wave will spawn, in number of waves.
	//  OUT : void
	//
	//  Sets up the Information for the Boss Wave, what should spawn and when. 
	void SetBossWave(MinionType _boss, unsigned int _frequency);
	// ================ //

	// === Friends
	friend void BossEvent::OnEnable();
	friend void BossEvent::OnDisable();
};