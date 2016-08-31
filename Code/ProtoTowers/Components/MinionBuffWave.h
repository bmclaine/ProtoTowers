#pragma once

#include "Components\IComponent.h"
#include "InfernoTimer.h"

struct Material;

class MinionBuffWave : public IComponent
{
	InfernoTimer m_itScaleTimer;
	Material* m_pMaterial;
	float m_fRadius;

	virtual ~MinionBuffWave() override;

	MinionBuffWave& operator=(const MinionBuffWave& rhs);
	virtual IComponent* CreateCopy(GameObject* const gameObject, Transform* const transform) override;

public:
	MinionBuffWave(GameObject* const gameObject, Transform* const transform);

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

	// IN: void
	// OUT: void
	//
	// Called when the Component becaomes enabled, whether its from just the component being Enabled, or the GameObject it's attached to.
	virtual void OnEnable() override;
	// IN: void
	// OUT: void
	//
	// Called when the Component becaomes disabled, whether its from just the component being Disabled, or the GameObject it's attached to.
	virtual void OnDisable() override;

	// IN: void
	// OUT: void
	//
	// Update that is called every frame
	virtual void Update() override;
};

