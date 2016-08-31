#pragma once

#include "Components\IComponent.h"
#include "GameObject.h"

class WeaponBuffScript : public IComponent
{
private:
	SkinnedMeshRenderer* m_pSkinnedRenderer;
	Transform* m_pParent;
	Matrix4 m_mOffsetMatrix;

	float m_fBuffTime;
	float m_fShotSpeedIncrease;

	virtual ~WeaponBuffScript() override;

	WeaponBuffScript& operator=(const WeaponBuffScript& rhs);
	virtual IComponent* CreateCopy(GameObject* const gameObject, Transform* const transform) override;

public:
	WeaponBuffScript(GameObject* const gameObject, Transform* const transform);

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

	// IN: void
	// OUT: void
	//
	// Update that is called every frame
	virtual void Update() override;

	void SetBuffTime(float time);
	void SetShotSpeedIncrease(float increase);

	friend WeaponBuffScript* GameObject::AddComponent();
};

