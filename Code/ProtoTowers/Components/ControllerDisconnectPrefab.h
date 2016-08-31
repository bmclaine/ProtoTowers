#pragma once

#include "Components\IComponent.h"

class Texture;

class ControllerDisconnectPrefab : public IComponent
{
private:
	Texture* m_pBackground;
	Texture* m_pDpDwnImageN;
	Texture* m_pDpDwnImageH;
	Texture* m_pDpDwnImageS;
	Texture* m_pOKText;

	virtual ~ControllerDisconnectPrefab() override;

	ControllerDisconnectPrefab& operator=(ControllerDisconnectPrefab&);
	virtual IComponent* CreateCopy(GameObject* const gameObject, Transform* const transform) override;

public:
	ControllerDisconnectPrefab(GameObject* const gameObject, Transform* const transform);

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
};

