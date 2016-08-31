#include "stdafx.h"
#include "AfterCutscenePrompt.h"
#include "CutScene.h"
#include "TutorialPrompt.h"

AfterCutscenePrompt::AfterCutscenePrompt(GameObject* const gameObject, Transform* const transform) : IComponent(gameObject, transform)
{
	m_pCutscene = nullptr;
	m_pPrompt = nullptr;
}

AfterCutscenePrompt::~AfterCutscenePrompt()
{
}

AfterCutscenePrompt& AfterCutscenePrompt::operator=(const AfterCutscenePrompt& rhs)
{
	return *this;
}
IComponent* AfterCutscenePrompt::CreateCopy(GameObject* const gameObject, Transform* const transform)
{
	return new AfterCutscenePrompt(gameObject, transform);
}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes any private, non object variables
void AfterCutscenePrompt::Init()
{
	m_pCutscene = nullptr;
	m_pPrompt = nullptr;
}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes object variables
void AfterCutscenePrompt::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{
	ObjectData* data = dataMap.find(GetInstanceID())->second;

	std::string propName = "Cutscene";
	Property* prop = data->GetProperty(propName);
	if (prop)
	{
		int id = atoi(prop->value.c_str());

		if (id > 0)
			m_pCutscene = (CutScene*)objectMap.find(id)->second;
	}

	propName = "Prompt";
	prop = data->GetProperty(propName);
	if (prop)
	{
		int id = atoi(prop->value.c_str());

		if (id > 0)
			m_pPrompt = (TutorialPrompt*)objectMap.find(id)->second;
	}
}

void AfterCutscenePrompt::OnEnable()
{
	if (m_pCutscene)
		m_pCutscene->SetOnCutSceneEndFunction(BindOnCutSceneEndFunction(&AfterCutscenePrompt::OnCutsceneEnd, this));
}
void AfterCutscenePrompt::OnDisable()
{
}

// Events
void AfterCutscenePrompt::OnCutsceneEnd()
{
	if (m_pPrompt)
		m_pPrompt->StartPrompt();
}