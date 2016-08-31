#pragma once

#include "Components\IComponent.h"
#include "InfernoTimer.h"

class SpriteRenderer;

class HealthPickup : public IComponent{

private:
	float m_HealPercent;
	InfernoTimer m_SpawnTimer;
	SpriteRenderer* m_pSpawnIcon;
	bool m_Active;

	void Reactivate();
	void Deactivate();

protected:
	virtual IComponent* CreateCopy(GameObject* const gameobject, Transform* const transform) override;
	HealthPickup& operator=(const HealthPickup& _rhs);

public:
	HealthPickup(GameObject* const gameObject, Transform* const transform);

	virtual ~HealthPickup()override;

	IComponent& operator=(IComponent&) override;

	void OnTriggerEnter(Collider * _col) override;

	virtual void Init();

	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap);

	virtual void OnEnable();

	void Update() override;

};