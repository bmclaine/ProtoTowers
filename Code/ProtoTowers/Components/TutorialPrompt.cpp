#include "stdafx.h"
#include "TutorialPrompt.h"
#include "Components\CutScene.h"
#include "Components\CutSceneWaypoint.h"

#include "ModuleInterface.h"
#include "../InfernoUI.h"

TutorialPrompt::TutorialPrompt(GameObject* const gameObject, Transform* const transform) : IComponent(gameObject, transform)
{
	m_pController = nullptr;
	m_pCutscene = nullptr;
	m_pNextWaypoint = nullptr;
	m_pPromptElement = nullptr;
	m_pPromptRenderer = nullptr;
	m_iCurrentPrompt = -1;
}

TutorialPrompt::~TutorialPrompt()
{
	size_t count = m_vKeyboardPrompts.size();
	for (size_t i = 0; i < count; i++)
		delete m_vKeyboardPrompts[i];
	m_vKeyboardPrompts.clear();

	count = m_vControllerPrompts.size();
	for (size_t i = 0; i < count; i++)
		delete m_vControllerPrompts[i];
	m_vControllerPrompts.clear();
}

TutorialPrompt& TutorialPrompt::operator=(const TutorialPrompt& rhs)
{
	m_pController = rhs.m_pController;
	m_pCutscene = rhs.m_pCutscene;
	m_pNextWaypoint = rhs.m_pNextWaypoint;

	size_t count = m_vControllerPrompts.size();
	for (size_t i = 0; i < count; i++)
		delete m_vControllerPrompts[i];
	m_vControllerPrompts.clear();

	count = rhs.m_vControllerPrompts.size();
	m_vControllerPrompts.reserve(count);
	for (size_t i = 0; i < count; i++)
	{
		Texture* tex = new Texture();
		Texture* copyText = rhs.m_vControllerPrompts[i];
		(*tex) = (*copyText);
		m_vControllerPrompts.push_back(tex);
	}

	count = m_vKeyboardPrompts.size();
	for (size_t i = 0; i < count; i++)
		delete m_vKeyboardPrompts[i];
	m_vKeyboardPrompts.clear();

	count = rhs.m_vKeyboardPrompts.size();
	m_vKeyboardPrompts.reserve(count);
	for (size_t i = 0; i < count; i++)
	{
		Texture* tex = new Texture();
		Texture* copyText = rhs.m_vKeyboardPrompts[i];
		(*tex) = (*copyText);
		m_vKeyboardPrompts.push_back(tex);
	}

	return *this;
}
IComponent* TutorialPrompt::CreateCopy(GameObject* const gameObject, Transform* const transform)
{
	TutorialPrompt* newTutorial = new TutorialPrompt(gameObject, transform);
	(*newTutorial) = (*this);

	return newTutorial;
}

void TutorialPrompt::OnEnable()
{
	using namespace Inferno_UI;

	UICanvas* tutorialCanvas = nullptr;
	unsigned int tutorialCanvasSlot;

	tutorialCanvas = GetCanvasByName("Tutorial");
	if (tutorialCanvas == nullptr)
		CreateNewCanvas("Tutorial", &tutorialCanvas, &tutorialCanvasSlot);

	m_pPromptElement = new UIElement();
	RectTransform* transform = m_pPromptElement->GetTransform();
	transform->SetAnchors(Anchors::ANCHOR_CENTER);
	transform->SetBounds(Vector2(640, 480));

	m_pPromptRenderer = m_pPromptElement->AddComponent<UIRenderer>();
	m_pPromptRenderer->SetLayer(50);

	AddToView(m_pPromptElement, "Tutorial");

	m_pPromptElement->SetActive(false);
}
void TutorialPrompt::OnDisable()
{
	InputManager::UnregisterEventFromAction("Interact", this, BindActionFunction(&TutorialPrompt::OnInteractHit, this));
	InputManager::UnregisterEventFromControlChange(this, BindControlChangeFunction(&TutorialPrompt::OnControlChange, this));
}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes any private, non object variables
void TutorialPrompt::Init()
{
	m_pController = nullptr;
	m_pCutscene = nullptr;
	m_pNextWaypoint = nullptr;
	m_pPromptElement = nullptr;
	m_pPromptRenderer = nullptr;

	size_t count = m_vKeyboardPrompts.size();
	for (size_t i = 0; i < count; i++)
		delete m_vKeyboardPrompts[i];
	m_vKeyboardPrompts.clear();

	count = m_vControllerPrompts.size();
	for (size_t i = 0; i < count; i++)
		delete m_vControllerPrompts[i];
	m_vControllerPrompts.clear();

	int m_iCurrentPrompt = -1;
	bool m_bUsingController = false;
}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes object variables
void TutorialPrompt::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{
	ObjectData* data = dataMap.find(GetInstanceID())->second;

	std::string propName = "PlayerController";
	Property* prop = data->GetProperty(propName);
	if (prop)
	{
		int id = atoi(prop->value.c_str());

		if (id > 0)
			m_pController = (PlayerController*)objectMap.find(id)->second;
	}

	propName = "CutScene";
	prop = data->GetProperty(propName);
	if (prop)
	{
		int id = atoi(prop->value.c_str());

		if (id > 0)
			m_pCutscene = (CutScene*)objectMap.find(id)->second;
	}

	propName = "NextWaypoint";
	prop = data->GetProperty(propName);
	if (prop)
	{
		int id = atoi(prop->value.c_str());

		if (id > 0)
			m_pNextWaypoint = (CutSceneWaypoint*)objectMap.find(id)->second;
	}

	UINT count = 0;
	propName = "ImageCount";
	prop = data->GetProperty(propName);
	if (prop)
		count = atoi(prop->value.c_str());

	for (UINT i = 0; i < m_vControllerPrompts.size(); i++)
		SAFE_DELETE(m_vControllerPrompts[i]);
	m_vControllerPrompts.clear();

	for (UINT i = 0; i < m_vKeyboardPrompts.size(); i++)
		SAFE_DELETE(m_vKeyboardPrompts[i]);
	m_vKeyboardPrompts.clear();

	m_vControllerPrompts.reserve(count);
	m_vKeyboardPrompts.reserve(count);

	for (UINT i = 0; i < count; i++)
	{
		propName = "ControllerPrompt" + std::to_string(i);
		prop = data->GetProperty(propName);
		if (prop)
		{
			Texture* tex = new Texture();
			tex->SetImage(InfernoEngine_Graphics::GetImage(Inferno::ToWideString(prop->value)));
			m_vControllerPrompts.push_back(tex);
		}

		propName = "KeyboardPrompt" + std::to_string(i);
		prop = data->GetProperty(propName);
		if (prop)
		{
			Texture* tex = new Texture();
			tex->SetImage(InfernoEngine_Graphics::GetImage(Inferno::ToWideString(prop->value)));
			m_vKeyboardPrompts.push_back(tex);
		}
	}
}

// IN: void
// OUT: void
//
// Called on the first frame that collision with a trigger is detected
void TutorialPrompt::OnTriggerEnter(Collider* col)
{
	if (col->gameObject->GetTag() == "Player")
		StartPrompt();
}

void TutorialPrompt::StartPrompt()
{
	if (m_vControllerPrompts.size() == 0 || m_vKeyboardPrompts.size() == 0)
		return;

	if (m_pController)
		m_pController->SetIgnore(IGNORE_ALL, true);

	if (m_pCutscene)
	{
		if (m_pNextWaypoint)
		{
			if (m_pCutscene->GetStartWaypoint())
				m_pCutscene->GetStartWaypoint()->SetNextWaypoint(m_pNextWaypoint);
			else
				m_pCutscene->SetStartWaypoint(m_pNextWaypoint);

			m_pCutscene->SetIsSkipable(false);
		}

		m_pCutscene->StartCutScene();
	}

	InputManager::RegisterEventToActionBinding("Interact", this, BindActionFunction(&TutorialPrompt::OnInteractHit, this));
	InputManager::RegisterEventToControlChange(this, BindControlChangeFunction(&TutorialPrompt::OnControlChange, this));
	m_bUsingController = InputManager::UsingController();

	if (m_pPromptElement && m_pPromptRenderer)
	{
		m_iCurrentPrompt = 0;
		m_pPromptRenderer->SetTexture(m_bUsingController ? m_vControllerPrompts[m_iCurrentPrompt] : m_vKeyboardPrompts[m_iCurrentPrompt]);
		m_pPromptElement->SetActive(true);
	}
}
void TutorialPrompt::EndPrompt()
{
	if (m_vControllerPrompts.size() == 0 || m_vKeyboardPrompts.size() == 0)
		return;

	if (m_pController)
		m_pController->SetIgnore(IGNORE_ALL, false);

	if (m_pCutscene)
		m_pCutscene->EndCutScene();

	InputManager::UnregisterEventFromAction("Interact", this, BindActionFunction(&TutorialPrompt::OnInteractHit, this));
	InputManager::UnregisterEventFromControlChange(this, BindControlChangeFunction(&TutorialPrompt::OnControlChange, this));

	m_iCurrentPrompt = -1;

	if (m_pPromptElement)
		m_pPromptElement->SetActive(false);

	Inferno::Destroy(gameObject);
}

// Events
void TutorialPrompt::OnInteractHit(ACTION_STATUS status)
{
	if (status != ACTION_STATUS::AS_PRESSED || m_iCurrentPrompt < 0 || m_iCurrentPrompt >= m_vControllerPrompts.size())
		return;

	m_iCurrentPrompt++;
	if (m_iCurrentPrompt >= (int)m_vControllerPrompts.size())
		EndPrompt();
	else
		m_pPromptRenderer->SetTexture(m_bUsingController ? m_vControllerPrompts[m_iCurrentPrompt] : m_vKeyboardPrompts[m_iCurrentPrompt]);
}

void TutorialPrompt::OnControlChange(bool usingController)
{
	if (m_iCurrentPrompt < 0 || m_iCurrentPrompt >= (int) m_vControllerPrompts.size())
		return;

	m_bUsingController = usingController;
	m_pPromptRenderer->SetTexture(m_bUsingController ? m_vControllerPrompts[m_iCurrentPrompt] : m_vKeyboardPrompts[m_iCurrentPrompt]);
}