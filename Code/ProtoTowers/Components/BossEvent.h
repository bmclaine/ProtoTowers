#pragma once
#include "Components\IComponent.h"

#include <vector>

// === Forward Declarations
class BossInfo;
class BossMinion;
class MinionSpawner;
class CutScene;

class BossEvent : public IComponent
{
private:
	// === Variables
	std::vector<MinionSpawner*> m_vSpawners;
	BossInfo* m_pBossInfo;
	BossMinion* m_pBoss;
	MinionType m_eBossType;
	CutScene* m_pBossCutScene;
	Transform* m_pCutSceneSpawn;
	unsigned int m_uiFrequency;
	unsigned int m_uiBossSpawner;
	unsigned int m_uiInitialWaveDelay;
	unsigned int m_uiBossCount;
	int m_iWaveCounter;
	bool m_bWaveSpawned;
//	bool m_bFirstTimeSpawn;

	// === Private Interface === //
	void CreateBossInfo();
	// ========================= //

	// === Events === //
	void OnBossKilled(Entity* _boss);
	void OnSpawnerDestroyed(Entity* _spawner);
	// ============== //

protected:
	// === Protected Interface === //
	virtual IComponent& operator=(IComponent& _rhs) override;
	BossEvent& operator=(const BossEvent& _rhs);
	virtual IComponent* CreateCopy(GameObject* const gameObject, Transform* const transform) override;
	// =========================== //

public:
	// === Constructor / Destructor === //
	BossEvent(GameObject* _owner, Transform* _transform);
	virtual ~BossEvent();
	// ================================ //

	// === Interface === //
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap) override;
	virtual void OnEnable() override;
	virtual void OnDisable() override;
	virtual void Update() override;
	void RegisterSpawner(MinionSpawner* _spawner);
	void AlertWaveSpawned();
	void StartBossSpawnCutScene();
	void EndBossSpawnCutScene();
	// ================= //

	// === Mutators === //
	void SetBossWave(MinionType _bossType, unsigned int _frequency);
	// ================ //
};

