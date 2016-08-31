#pragma once

#include "Components\IComponent.h"

class CutScene;
class TutorialPrompt;

class AfterCutscenePrompt : public IComponent
{
private:
	CutScene* m_pCutscene;
	TutorialPrompt* m_pPrompt;

	AfterCutscenePrompt& operator=(const AfterCutscenePrompt& rhs);
	virtual IComponent* CreateCopy(GameObject* const gameObject, Transform* const transform) override;

public:
	AfterCutscenePrompt(GameObject* const gameObject, Transform* const transform);
	~AfterCutscenePrompt();

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
	void OnCutsceneEnd();
};

