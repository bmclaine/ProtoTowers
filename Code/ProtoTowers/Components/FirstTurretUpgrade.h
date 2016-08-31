#pragma once

#include "Components\IComponent.h"

class FirstTurretUpgrade : public IComponent
{
private:
	int m_iChannel;

	FirstTurretUpgrade& operator=(const FirstTurretUpgrade& rhs);
	virtual IComponent* CreateCopy(GameObject* const gameObject, Transform* const transform) override;

public:
	FirstTurretUpgrade(GameObject* const gameObject, Transform* const transform);
	~FirstTurretUpgrade();

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

	virtual void OnEnable() override;
	virtual void OnDisable() override;

	// Events
	void OnTowerUpgrade(void* pData);
};

