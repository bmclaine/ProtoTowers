#pragma once

#include "Components\IComponent.h"

#include "InfernoTimer.h"

class BaseTower;
class TowerNode;

class FirstTurret : public IComponent
{
	InfernoTimer m_itAfterTowerBuild;
	TowerNode* m_pTowerNode;
	int m_iChannel;

	FirstTurret& operator=(const FirstTurret& rhs);
	virtual IComponent* CreateCopy(GameObject* const gameObject, Transform* const transform) override;

public:
	FirstTurret(GameObject* const gameObject, Transform* const transform);
	virtual ~FirstTurret() override;

	// IN: void
	// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
	//
	// Initializes any private, non object variables
	virtual void Init() override;

	// IN: void
	// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
	//
	// Initializes object variables
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap) override;

	void OnTowerBuilt(BaseTower*);
	void OnWaitTimeEnd();
};

