#include "stdafx.h"
#include "SkipTutorial.h"
#include "Assets\Texture.h"
#include "Components\SpriteRenderer.h"
#include "InputManager.h"
#include "EventSystem.h"

#include "ModuleInterface.h"

SkipTutorial::SkipTutorial(GameObject* const gameObject, Transform* const transform) : IComponent(gameObject, transform)
{
	m_pInteractImage = nullptr;
	m_pXboxInteract = new Texture();
	m_pXboxInteract->SetImage(Inferno::GetImage(L"Tutorail_Skip_Xbox.dds"));
	m_pKeyInteract = new Texture();
	m_pKeyInteract->SetImage(Inferno::GetImage(L"Tutorail_Skip_Key.dds"));

	m_bIgnoreInput = false;
}

SkipTutorial::~SkipTutorial()
{
	SAFE_DELETE(m_pXboxInteract);
	SAFE_DELETE(m_pKeyInteract);
}

SkipTutorial& SkipTutorial::operator=(const SkipTutorial& rhs)
{
	return *this;
}
IComponent* SkipTutorial::CreateCopy(GameObject* const gameObject, Transform* const transform)
{
	return new SkipTutorial(gameObject, transform);
}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes any private, non object variables
void SkipTutorial::Init() {}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes object variables
void SkipTutorial::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{
	ObjectData* data = dataMap.find(GetInstanceID())->second;

	std::string propName = "NextLevel";
	Property* prop = data->GetProperty(propName);
	if (prop)
		m_sNextLevel = prop->value;

	m_pInteractImage = gameObject->GetComponentInChildren<SpriteRenderer>();

	if (m_pInteractImage)
		m_pInteractImage->gameObject->SetActive(false, true);
}

// IN: void
// OUT: void
//
// Called on the first frame that collision with a trigger is detected
void SkipTutorial::OnTriggerEnter(Collider* col)
{
	if (col->gameObject->GetTag() == "Player")
	{
		if (m_pInteractImage)
		{
			m_pInteractImage->gameObject->SetActive(true);
			OnControlChange(InputManager::UsingController());
			InputManager::RegisterEventToControlChange(this, BindControlChangeFunction(&SkipTutorial::OnControlChange, this));
		}

		InputManager::RegisterEventToActionBinding("Interact", this, BindActionFunction(&SkipTutorial::OnInteract, this));
	}
}
// IN: void
// OUT: void
//
// Called when collision is no longer detected with a trigger
void SkipTutorial::OnTriggerExit(Collider* col)
{
	if (col->gameObject->GetName() == "Player")
	{
		if (m_pInteractImage)
		{
			m_pInteractImage->gameObject->SetActive(false);
			InputManager::UnregisterEventFromControlChange(this, BindControlChangeFunction(&SkipTutorial::OnControlChange, this));
		}

		InputManager::UnregisterEventFromAction("Interact", this, BindActionFunction(&SkipTutorial::OnInteract, this));
	}

}

// IN: void
// OUT: void
//
// Called when the Component becaomes enabled, whether its from just the component being Enabled, or the GameObject it's attached to.
void SkipTutorial::OnEnable()
{
	EventSystem::GetInstance()->RegisterForEvent("TogglePauseMenu", this, std::bind(&SkipTutorial::OnPauseToggle, this, std::placeholders::_1));
}
// IN: void
// OUT: void
//
// Called when the Component becaomes disabled, whether its from just the component being Disabled, or the GameObject it's attached to.
void SkipTutorial::OnDisable()
{
	InputManager::UnregisterEventFromControlChange(this, BindControlChangeFunction(&SkipTutorial::OnControlChange, this));
	InputManager::UnregisterEventFromAction("Interact", this, BindActionFunction(&SkipTutorial::OnInteract, this));
	EventSystem::GetInstance()->UnregisterFromEvent("TogglePauseMenu", this, std::bind(&SkipTutorial::OnPauseToggle, this, std::placeholders::_1));
}

// Events
void SkipTutorial::OnControlChange(bool usingController)
{
	m_pInteractImage->SetImage(usingController ? m_pXboxInteract->GetImage() : m_pKeyInteract->GetImage());
}
void SkipTutorial::OnInteract(ACTION_STATUS status)
{
	if (status != ACTION_STATUS::AS_PRESSED || (m_pInteractImage && !m_pInteractImage->isEnabled()) || m_sNextLevel == "" || m_bIgnoreInput)
		return;

	InfernoEngine_SceneManagement::LoadScene(m_sNextLevel.c_str());
}
void SkipTutorial::OnPauseToggle(void* pData)
{
	m_bIgnoreInput = *(bool*)pData;
}