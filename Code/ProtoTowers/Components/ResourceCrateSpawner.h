#pragma once

class IComponent;

class ResourceCrateSpawner : public IComponent
{
private:
	InfernoTimer m_itRespawnTimer;
	int m_iSpawnChannel;
	float m_fSpawnTime;
	bool m_bHasSpawned;
protected:
	virtual IComponent* CreateCopy(GameObject* const _gameObject, Transform* const _transform) override;

public:
	ResourceCrateSpawner(GameObject* const _gameObject, Transform* const _transform);

	virtual ~ResourceCrateSpawner()override;

	IComponent& operator=(IComponent&) override;
	ResourceCrateSpawner& operator=(const ResourceCrateSpawner& _rhs);
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap);
	virtual void Init() override;

	virtual void OnEnable() override;
	virtual void OnDisable() override;

	virtual void Update() override;

	void StartSpawnTimer(void *);
	void SpawnNewCrate();

};