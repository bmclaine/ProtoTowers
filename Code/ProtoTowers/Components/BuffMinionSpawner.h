#pragma once

#include "Components\IComponent.h"
#include "Components\CutSceneWaypoint.h"
#include "InfernoTimer.h"

class MinionSpawnPoint;
class AlertMessage;
class SpriteRenderer;
class CutSceneWaypoint;
class Camera;

class BuffMinionSpawner : public IComponent{

private:
	std::vector<MinionSpawnPoint*> m_vSpawnPoints;
	InfernoTimer m_SpawnTimer;
	InfernoTimer m_ActivateTimer;
	AlertMessage* m_pAlertMessage;
	SpriteRenderer* m_pActivateIcon;
	Transform* m_pCrystal;
	CutSceneWaypoint* m_pCutSceneWaypoint;
	Camera* m_pCutSceneCamera;
	Camera* m_pMainCamera;
	bool m_bActive;
	float m_fSpawnTimer;
	float m_fActivateTimer;
	float m_fMinionMaxHealth;
	float m_fMinionDamageMultiplier;

protected:
	BuffMinionSpawner& operator=(const BuffMinionSpawner& _rhs);
	virtual IComponent* CreateCopy(GameObject* const gameobject, Transform* const transform) override;

public:
	BuffMinionSpawner(GameObject* const gameObject, Transform* const transform);

	virtual ~BuffMinionSpawner()override;

	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap);

	virtual void OnEnable();

	virtual void OnDisable();

	virtual void OnDestroy();

	virtual void Update() override;

	virtual void OnTriggerEnter(Collider* _collider) override;

private:
	void Activate();
	void SendAlert();
	void SpawnMinions();
	void BuffMinion(GameObject* gameObject);
	void FindAlertMessage();
	void StartCutScene();
	void EndCutScene();
};