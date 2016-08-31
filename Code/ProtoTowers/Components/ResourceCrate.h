//Author: William Zane Lewis
//
//An environment object that spawns in resources when it is destroyed
#pragma once
#define NUM_RESOURCES_TO_SPAWN 3

class IComponent;
class GameObject;

class ResourceCrate : public IComponent 
{
private:
	bool m_bIsDead;
	int m_iSpawnChannel;
protected:
	virtual IComponent* CreateCopy(GameObject* const gameobject, Transform* const transform) override;
	ResourceCrate& operator=(const ResourceCrate& _rhs);
public:
	ResourceCrate(GameObject* const gameObject, Transform* const transform);

	virtual ~ResourceCrate()override;

	IComponent& operator=(IComponent&) override;

	virtual void OnEnable() override;

	virtual void OnDisable() override;

	void OnTriggerEnter(Collider * _col) override;

	void OnCollisionEnter(Collider* col) override;

	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap);

	void Update() override;

	void SpawnInResources();

	void SetChannel(int _spawnChannel);

};