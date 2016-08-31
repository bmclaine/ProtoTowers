//Author: William Zane Lewis
//
//Wave of minions to spawn after the barrier lowers

#ifndef _MINIONWAVESPAWN_H
#define _MINIONWAVESPAWN_H
class IComponent;
class GameObject;
class MinionSpawnPoint;
class AlertMessage;
class MinionWaveSpawn : public IComponent
{
private:
	int m_iChannel;
	int m_iNumMinions;
	int m_iNumMinionsSpawned;
	int m_iNumSpawnPoint;
	std::vector<MinionType> m_vMinionsToSpawn;
	std::vector<MinionSpawnPoint*> m_vSpawnPoints;
	bool m_bSpawnMinions;
	InfernoTimer m_itSpawnDelay;
	AlertMessage * m_pAlertMessage;
	std::string m_sMessage;
	void SpawnWave();
	void AddMinionType(MinionType _type);
	void AddSpawnPoint(MinionSpawnPoint* _spawnPoint);
	protected:
	virtual IComponent* CreateCopy(GameObject* const _gameobject, Transform* const _transform) override;

public:
	MinionWaveSpawn(GameObject* const gameObject, Transform* const transform);

	virtual ~MinionWaveSpawn()override;

	IComponent& operator=(IComponent&) override;
	MinionWaveSpawn& operator=(const MinionWaveSpawn& _rhs);
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap);
	virtual void Init() override;

	virtual void OnEnable() override;
	virtual void OnDisable() override;

	virtual void Update() override;
	
	void EnableSpawning(void *);

	std::string GetNameFromType(MinionType _type);

};

#endif