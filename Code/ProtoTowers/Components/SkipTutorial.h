#pragma once

#include "Components\IComponent.h"

class Texture;
class SpriteRenderer;

class SkipTutorial : public IComponent
{
private:
	std::string m_sNextLevel;
	Texture* m_pXboxInteract;
	Texture* m_pKeyInteract;
	SpriteRenderer* m_pInteractImage;
	bool m_bIgnoreInput;

	SkipTutorial& operator=(const SkipTutorial& rhs);
	virtual IComponent* CreateCopy(GameObject* const gameObject, Transform* const transform) override;

public:
	SkipTutorial(GameObject* const gameObject, Transform* const transform);
	virtual ~SkipTutorial() override;

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
	// Called on the first frame that collision with a trigger is detected
	virtual void OnTriggerEnter(Collider*);
	// IN: void
	// OUT: void
	//
	// Called when collision is no longer detected with a trigger
	virtual void OnTriggerExit(Collider*);

	// IN: void
	// OUT: void
	//
	// Called when the Component becaomes enabled, whether its from just the component being Enabled, or the GameObject it's attached to.
	virtual void OnEnable();
	// IN: void
	// OUT: void
	//
	// Called when the Component becaomes disabled, whether its from just the component being Disabled, or the GameObject it's attached to.
	virtual void OnDisable();

	// Events
	void OnControlChange(bool usingController);
	void OnInteract(ACTION_STATUS status);
	void OnPauseToggle(void* pData);
};

