#include "stdafx.h"
#include "PauseMenu.h"

#include "../InfernoUI.h"
#include "EventSystem.h"
#include "InputManager.h"
#include "ModuleInterface.h"

// === Constructor / Destructor === //
PauseMenu::PauseMenu(UIElement* _owner) : UIComponent(_owner)
{
	m_pSelectImage = nullptr;
	m_pBackImage = nullptr;

	m_pSelectBtnTexture = new Texture();
	m_pSelectBtnTexture->SetImage(Inferno::GetImage(L"UI_XboxBtn_A.dds"));
	m_pBackBtnTexture = new Texture();
	m_pBackBtnTexture->SetImage(Inferno::GetImage(L"UI_XboxBtn_B.dds"));

	m_bAllInputToggled = true;
}

PauseMenu::~PauseMenu()
{
	delete m_pSelectBtnTexture;
	delete m_pBackBtnTexture;

	OnDisable();
}
// ================================ //

// === Interface === //
void PauseMenu::PostInit()
{
	m_pMenuElement = GetTransform()->GetChildByName(std::string("PauseMenu Element"));
	if (!ASSERT(m_pMenuElement != nullptr, "No PauseMenu Element found, make sure the Element is named correctly.")) {
		return;
	}

	m_pMenuElement->SetActive(false);

	if (m_pSelectImage == nullptr) {
		UIElement* controlsInfo = GetTransform()->GetChildByName(std::string("PM Controls Info"), true);
		if (!ASSERT(controlsInfo != nullptr, "No Control Info Element found on the Pause Menu, make sure it's named correctly.")) {
			return;
		}

#pragma region ControlImages_Setup
		RectTransform* transform;
		UIRenderer* image;

		// === Select Btn Image
		UIElement* selectImg = new UIElement();
		transform = selectImg->GetTransform();
		transform->SetParent(controlsInfo->GetTransform());
		transform->SetPosition(-180.0f, 0.0f);
		transform->SetBounds(Vector2(64.0f, 64.0f));
		//// Components
			// == UIRenderer (Select Img)
			image = selectImg->AddComponent<UIRenderer>();
			image->SetTexture(m_pSelectBtnTexture);
			image->SetColor(Color(0.8f, 0.8f, 0.8f, 1.0f));

		// === Back Btn Image
		UIElement* backImg = new UIElement();
		transform = backImg->GetTransform();
		transform->SetParent(controlsInfo->GetTransform());
		transform->SetPosition(20.0f, 0.0f);
		transform->SetBounds(Vector2(64.0f, 64.0f));
		//// Components
			// == UIRenderer (Back Img)
			image = backImg->AddComponent<UIRenderer>();
			image->SetTexture(m_pBackBtnTexture);
			image->SetColor(Color(0.8f, 0.8f, 0.8f, 1.0f));
#pragma endregion
	}
}

void PauseMenu::OnEnable()
{
	using namespace InfernoEngine_Input;
	RegisterEventToActionBinding("Pause", this, BindActionFunction(&PauseMenu::OnTogglePauseMenu, this));
	RegisterEventToActionBinding("Escape", this, BindActionFunction(&PauseMenu::OnTogglePauseMenu, this));

	EventSystem* pEventSystem = EventSystem::GetInstance();
	pEventSystem->RegisterForEvent("TogglePauseMenuInput", this, BindEventFunction(&PauseMenu::OnTogglePauseMenuInput, this));
	pEventSystem->RegisterForEvent("TogglePauseMenuFullInput", this, BindEventFunction(&PauseMenu::OnTogglePauseMenuFullInput, this));

	Inferno_UI::SetCurrentSelectable(m_pFirstSelectable);
}

void PauseMenu::OnDisable()
{
	using namespace InfernoEngine_Input;
	UnregisterEventFromActionBinding("Pause", this, BindActionFunction(&PauseMenu::OnTogglePauseMenu, this));
	UnregisterEventFromActionBinding("Escape", this, BindActionFunction(&PauseMenu::OnTogglePauseMenu, this));

	EventSystem* pEventSystem = EventSystem::GetInstance();
	pEventSystem->UnregisterFromEvent("TogglePauseMenuInput", this, BindEventFunction(&PauseMenu::OnTogglePauseMenuInput, this));
	pEventSystem->UnregisterFromEvent("TogglePauseMenuFullInput", this, BindEventFunction(&PauseMenu::OnTogglePauseMenuFullInput, this));

	InfernoEngine_Input::UnregisterEventFromActionBinding("SecondaryInteract", this, BindActionFunction(&PauseMenu::OnTogglePauseMenu, this));
}
// ================= //

// ===== Events ===== //

// === OnTogglePauseMenuInput
//  IN : void* - Event Data, this should just have a bool stored inside of it
//  OUT : void
//
//  An event that will be invoked when the pause menu should toggle itself on or off. This would be for cases where another UI menu is up, where the 
//  pause menu should not react to certain input
void PauseMenu::OnTogglePauseMenuInput(void* _pData)
{
	bool toggleValue = *((bool*)_pData);
	if (toggleValue && !m_bAllInputToggled) {
		m_bAllInputToggled = true;

		using namespace InfernoEngine_Input;
		RegisterEventToActionBinding("Escape", this, BindActionFunction(&PauseMenu::OnTogglePauseMenu, this));
	}
	else if (!toggleValue && m_bAllInputToggled) {
		m_bAllInputToggled = false;

		using namespace InfernoEngine_Input;
		UnregisterEventFromActionBinding("Escape", this, BindActionFunction(&PauseMenu::OnTogglePauseMenu, this));
	}
}

// === OnTogglePauseMenuFullInput
//  IN : void* - Event Data that just has a bool, for whether or not to toggle the FullInput of the PauseMenu
//  OUT : void
//
//  Toggles the Full Input of the PauseMenu, meaning that if it's turned off, the Pause Menu cannot be activated at all
void PauseMenu::OnTogglePauseMenuFullInput(void* _pData)
{
	bool toggleValue = *((bool*)_pData);
	if (toggleValue && !m_bAllInputToggled) {
		m_bAllInputToggled = true;

		using namespace InfernoEngine_Input;
		RegisterEventToActionBinding("Pause", this, BindActionFunction(&PauseMenu::OnTogglePauseMenu, this));
		RegisterEventToActionBinding("Escape", this, BindActionFunction(&PauseMenu::OnTogglePauseMenu, this));
	}
	else if (!toggleValue && m_bAllInputToggled) {
		m_bAllInputToggled = false;

		using namespace InfernoEngine_Input;
		UnregisterEventFromActionBinding("Pause", this, BindActionFunction(&PauseMenu::OnTogglePauseMenu, this));
		UnregisterEventFromActionBinding("Escape", this, BindActionFunction(&PauseMenu::OnTogglePauseMenu, this));
	}
}

// === OnTogglePauseMenu
//  IN : ACTION_STATUS - Status of the Input Binding
//  OUT : void
//
//  Input Event function for toggling the Pause Menu on and Off.
void PauseMenu::OnTogglePauseMenu(ACTION_STATUS _status)
{
	if (_status == ACTION_STATUS::AS_PRESSED) {
		if (m_pMenuElement->IsActive()) {
			// === Pause Menu is open, close it
			OnResumeBtnClick();
		}
		else {
			// === Pause Menu is closed, open it
			Time::TimeScale = 0.0f;
			bool toggle = true;

			EventSystem::GetInstance()->SendEventNow("TogglePauseMenu", &toggle);

			m_pMenuElement->SetActive(true);

			InfernoEngine_Input::RegisterEventToActionBinding("SecondaryInteract", this, BindActionFunction(&PauseMenu::OnTogglePauseMenu, this));
			
			bool data = false;
			EventSystem* pEventSystem = EventSystem::GetInstance();
			pEventSystem->SendEventNow("PlayerRecieveInput", (void*)&data);
			pEventSystem->SendEventNow("HelpScreenRecieveInput", (void*)&data);
			pEventSystem->SendEventNow("CloseTowerMenu", nullptr);
			pEventSystem->SendEventNow("ShowCrosshair", (void*)&data);

			Inferno_UI::SetCurrentSelectable(m_pFirstSelectable);

			InputManager::s_CenterMouse = false;
			InputManager::ShowMouse(true);

			// === Get the Right Control Scheme
			if (InputManager::UsingController()) {
				m_pSelectBtnTexture->SetImage(Inferno::GetImage(L"UI_XboxBtn_A.dds"));
				m_pBackBtnTexture->SetImage(Inferno::GetImage(L"UI_XboxBtn_B.dds"));
			}
			else {
				m_pSelectBtnTexture->SetImage(Inferno::GetImage(L"UI_KeyBtn_E.dds"));
				m_pBackBtnTexture->SetImage(Inferno::GetImage(L"UI_KeyBtn_Esc.dds"));
			}
		}
	}
}
// ================== //

// ===== Button Functions ===== //
void PauseMenu::OnResumeBtnClick()
{
	Time::TimeScale = 1.0f;
	bool* toggle = new bool(false);

	EventSystem::GetInstance()->QueueEvent("TogglePauseMenu", toggle);

	m_pMenuElement->SetActive(false);

	InfernoEngine_Input::UnregisterEventFromActionBinding("SecondaryInteract", this, BindActionFunction(&PauseMenu::OnTogglePauseMenu, this));

	bool data = true;
	EventSystem* pEventSystem = EventSystem::GetInstance();
	pEventSystem->SendEventNow("PlayerRecieveInput", (void*)&data);
	pEventSystem->SendEventNow("HelpScreenRecieveInput", (void*)&data);
	pEventSystem->SendEventNow("ShowCrosshair", (void*)&data);

	Inferno_UI::SetCurrentSelectable(nullptr);

#if !defined(_DEBUG) 
	InputManager::s_CenterMouse = true;
	InputManager::ShowMouse(false);
#endif
}

void PauseMenu::OnOptionsBtnClick()
{
	using namespace Inferno_UI;

	UICanvas* menuCanvas = GetCanvasByName("PauseHUD");
	UICanvas* optionsCanvas = GetCanvasByName("Options");

	if (menuCanvas && optionsCanvas)
	{
		menuCanvas->GetUIElement()->SetActive(false);
		//this->GetUIElement()->SetActive(false);
		optionsCanvas->GetUIElement()->SetActive(true);
	}
}

void PauseMenu::OnHowToPlayBtnClick()
{
	OnResumeBtnClick();
	bool* toggle = new bool(true);
	EventSystem::GetInstance()->QueueEvent("TogglePauseMenu", toggle);
	bool data = true;
	EventSystem::GetInstance()->SendEventNow("HelpScreenRecieveInput", (void*)&data);
	EventSystem::GetInstance()->SendEventNow("OpenHelpScreen", nullptr);
}

void PauseMenu::OnQuitBtnQuit()
{
	OnResumeBtnClick();
//	BaseState::SetState(BaseState::s_MainMenuState);
	InfernoEngine_SceneManagement::LoadScene("MainMenuScene");
}
// ============================ //

// ===== Mutators ===== //
void PauseMenu::SetFirstSelectable(UISelectable* _firstSelectable)
{
	m_pFirstSelectable = _firstSelectable;
}
// ==================== //