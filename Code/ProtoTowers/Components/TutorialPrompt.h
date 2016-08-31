#pragma once

#include "Components\IComponent.h"

class PlayerController;
class CutScene;
class CutSceneWaypoint;
class UIElement;
class UIRenderer;
class Texture;

class TutorialPrompt : public IComponent
{
	PlayerController* m_pController;
	CutScene* m_pCutscene;
	CutSceneWaypoint* m_pNextWaypoint;
	UIElement* m_pPromptElement;
	UIRenderer* m_pPromptRenderer;
	std::vector<Texture*> m_vKeyboardPrompts;
	std::vector<Texture*> m_vControllerPrompts;
	int m_iCurrentPrompt;
	bool m_bUsingController;

	TutorialPrompt& operator=(const TutorialPrompt& rhs);
	virtual IComponent* CreateCopy(GameObject* const gameObject, Transform* const transform) override;

public:
	TutorialPrompt(GameObject* const gameObject, Transform* const transform);
	virtual ~TutorialPrompt() override;

	virtual void OnEnable() override;
	virtual void OnDisable() override;

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
	virtual void OnTriggerEnter(Collider*) override;

	void StartPrompt();
	void EndPrompt();

	// Events
	void OnInteractHit(ACTION_STATUS status);
	void OnControlChange(bool usingController);
};