#include "stdafx.h"
#include "HelpScreen.h"

#include "../InfernoUI.h"
#include "EventSystem.h"
#include "InputManager.h"
#include "ModuleInterface.h"

// ===== Constructor / Destructor ===== //
HelpScreen::HelpScreen(UIElement* _owner) : UIComponent(_owner)
{
	m_pImage = nullptr;
	m_pCloseBtnImage = nullptr;
	m_pNextBtnImage = nullptr;
	m_pPrevBtnImage = nullptr;
	m_bRecieveInput = true;

	m_pHelpImages[0] = new Texture();
	m_pHelpImages[0]->SetImage(Inferno::GetImage(L"UI_F1Page_1.dds"));
	m_pHelpImages[1] = new Texture();
	m_pHelpImages[1]->SetImage(Inferno::GetImage(L"UI_F1Page_2.dds"));
	m_pHelpImages[2] = new Texture();
	m_pHelpImages[2]->SetImage(Inferno::GetImage(L"UI_F1Page_3.dds"));
	m_pHelpImages[3] = new Texture();
	m_pHelpImages[3]->SetImage(Inferno::GetImage(L"UI_F1Page_4.dds"));
	m_pHelpImages[4] = new Texture();
	m_pHelpImages[4]->SetImage(Inferno::GetImage(L"UI_F1Page_5.dds"));
	m_pHelpImages[5] = new Texture();
	m_pHelpImages[5]->SetImage(Inferno::GetImage(L"UI_F1Page_6.dds"));

	m_pCloseBtnTexture = new Texture();
	m_pCloseBtnTexture->SetImage(Inferno::GetImage(L"UI_XboxBtn_Back.dds"));
	m_pNextBtnTexture = new Texture();
	m_pNextBtnTexture->SetImage(Inferno::GetImage(L"UI_XboxBtn_RT.dds"));
	m_pPrevBtnTexture = new Texture();
	m_pPrevBtnTexture->SetImage(Inferno::GetImage(L"UI_XboxBtn_LT.dds"));
}

HelpScreen::~HelpScreen()
{
	OnDisable();
	delete m_pHelpImages[0];
	delete m_pHelpImages[1];
	delete m_pHelpImages[2];
	delete m_pHelpImages[3];
	delete m_pHelpImages[4];
	delete m_pHelpImages[5];
	delete m_pCloseBtnTexture;
	delete m_pNextBtnTexture;
	delete m_pPrevBtnTexture;
}
// ==================================== //

// ===== Interface ===== //
void HelpScreen::OnEnable()
{
	if (m_pImage == nullptr) {
#pragma region Screen_Setup
		// === Main Image
		m_pImage = m_pUIElement->AddComponent<UIRenderer>();
		m_pImage->SetLayer(12);
		m_pImage->SetEnabled(false);

		// === Children Images
			RectTransform* transform;
			// == Close Btn
			UIElement* closeBtn = new UIElement();
			transform = closeBtn->GetTransform();
			transform->SetParent(this->GetTransform());
			transform->SetAnchors(Anchors::ANCHOR_TOPRIGHT);
			transform->SetPosition(-120.0f, 40.0f);
			transform->SetBounds(Vector2(64.0f, 64.0f));
			//// Components
				// == UIRenderer
				m_pCloseBtnImage = closeBtn->AddComponent<UIRenderer>();
				m_pCloseBtnImage->SetTexture(m_pCloseBtnTexture);
				m_pCloseBtnImage->SetLayer(13);

			m_pCloseBtnImage->SetEnabled(false);

			// == Next Btn
			UIElement* nextBtn = new UIElement();
			transform = nextBtn->GetTransform();
			transform->SetParent(this->GetTransform());
			transform->SetAnchors(Anchors::ANCHOR_BOTTOMRIGHT);
			transform->SetPosition(-165.0f, -46.0f);
			transform->SetBounds(Vector2(64.0f, 64.0f));
			//// Components
				// == UIRenderer
				m_pNextBtnImage = nextBtn->AddComponent<UIRenderer>();
				m_pNextBtnImage->SetTexture(m_pNextBtnTexture);
				m_pNextBtnImage->SetLayer(13);

			m_pNextBtnImage->SetEnabled(false);

			// == Prev Btn
			UIElement* prevBtn = new UIElement();
			transform = prevBtn->GetTransform();
			transform->SetParent(this->GetTransform());
			transform->SetAnchors(Anchors::ANCHOR_BOTTOMLEFT);
			transform->SetPosition(215.0f, -46.0f);
			transform->SetBounds(Vector2(64.0f, 64.0f));
			//// Components
				// == UIRenderer
				m_pPrevBtnImage = prevBtn->AddComponent<UIRenderer>();
				m_pPrevBtnImage->SetTexture(m_pPrevBtnTexture);
				m_pPrevBtnImage->SetLayer(13);

			m_pPrevBtnImage->SetEnabled(false);
#pragma endregion
	}

	using namespace InfernoEngine_Input;
	RegisterEventToActionBinding("Help", this, BindActionFunction(&HelpScreen::OnToggleHelpScreen, this));

	EventSystem::GetInstance()->RegisterForEvent("OpenHelpScreen", this, BindEventFunction(&HelpScreen::OnOpenHelpScreen, this));
	EventSystem::GetInstance()->RegisterForEvent("HelpScreenRecieveInput", this, BindEventFunction(&HelpScreen::OnSetRecieveInput, this));
}

void HelpScreen::OnDisable()
{
	using namespace InfernoEngine_Input;
	UnregisterEventFromActionBinding("Help", this, BindActionFunction(&HelpScreen::OnToggleHelpScreen, this));

	EventSystem::GetInstance()->UnregisterFromEvent("OpenHelpScreen", this, BindEventFunction(&HelpScreen::OnOpenHelpScreen, this));
	EventSystem::GetInstance()->UnregisterFromEvent("HelpScreenRecieveInput", this, BindEventFunction(&HelpScreen::OnSetRecieveInput, this));
}

void HelpScreen::Update()
{
	m_fDelayTime -= Time::TrueDeltaTime;
}
// ===================== //

// ===== Input Events ===== //
void HelpScreen::OnToggleHelpScreen(ACTION_STATUS _status)
{
	if (m_pImage == nullptr || m_fDelayTime > 0.0f)
		return;

	if (m_pImage->IsEnabled()) {
		m_pImage->SetEnabled(false);
		m_pCloseBtnImage->SetEnabled(false);
		m_pNextBtnImage->SetEnabled(false);
		m_pPrevBtnImage->SetEnabled(false);

		Time::TimeScale = 1.0f;
		m_fDelayTime = 0.5f;

		using namespace InfernoEngine_Input;
		UnregisterEventFromAxisBinding("Next", this, BindAxisFunction(&HelpScreen::OnNextPage, this));
		UnregisterEventFromAxisBinding("Previous", this, BindAxisFunction(&HelpScreen::OnPrevPage, this));
		UnregisterEventFromActionBinding("Escape", this, BindActionFunction(&HelpScreen::OnToggleHelpScreen, this));

		bool data = true;
		bool* toggle = new bool(false);
		EventSystem::GetInstance()->SendEventNow("PlayerRecieveInput", (void*)&data);
		EventSystem::GetInstance()->SendEventNow("TogglePauseMenuFullInput", (void*)&data);
		EventSystem::GetInstance()->QueueEvent("TogglePauseMenu", (void*)toggle);

#if !defined(_DEBUG) 
		InputManager::s_CenterMouse = true;
		InputManager::ShowMouse(false);
#endif
	}
	else {
		OpenScreen();
	}
}

void HelpScreen::OnNextPage(float _axisValue)
{
	if (m_pImage == nullptr || m_fDelayTime > 0.0f)
		return;

	if (_axisValue > 0) {
		if (m_pImage->IsEnabled() && m_uiCurrentPage < 5) {
			++m_uiCurrentPage;
			m_pImage->SetTexture(m_pHelpImages[m_uiCurrentPage]);
			m_fDelayTime = 0.5f;

			if (m_uiCurrentPage == 5) {
				m_pNextBtnImage->SetEnabled(false);
			}
			else {
				m_pNextBtnImage->SetEnabled(true);
				m_pPrevBtnImage->SetEnabled(true);
			}
		}
	}
}

void HelpScreen::OnPrevPage(float _axisValue)
{
	if (m_pImage == nullptr || m_fDelayTime > 0.0f)
		return;

	if (_axisValue > 0) {
		if (m_pImage->IsEnabled() && m_uiCurrentPage > 0) {
			--m_uiCurrentPage;
			m_pImage->SetTexture(m_pHelpImages[m_uiCurrentPage]);
			m_fDelayTime = 0.5f;

			if (m_uiCurrentPage == 0) {
				m_pPrevBtnImage->SetEnabled(false);
			}
			else {
				m_pNextBtnImage->SetEnabled(true);
				m_pPrevBtnImage->SetEnabled(true);
			}
		}
	}
}
// ======================== //

// ===== Private Interface ===== //
void HelpScreen::OpenScreen()
{
	m_pImage->SetEnabled(true);
	m_pCloseBtnImage->SetEnabled(true);
	m_pNextBtnImage->SetEnabled(true);

	Time::TimeScale = 0.0f;
	m_fDelayTime = 0.5f;

	m_pImage->SetTexture(m_pHelpImages[0]);
	m_uiCurrentPage = 0;

	using namespace InfernoEngine_Input;
	RegisterEventToAxisBinding("Next", this, BindAxisFunction(&HelpScreen::OnNextPage, this));
	RegisterEventToAxisBinding("Previous", this, BindAxisFunction(&HelpScreen::OnPrevPage, this));
	RegisterEventToActionBinding("Escape", this, BindActionFunction(&HelpScreen::OnToggleHelpScreen, this));

	bool data = false;
	EventSystem::GetInstance()->SendEventNow("PlayerRecieveInput", (void*)&data);
	EventSystem::GetInstance()->SendEventNow("TogglePauseMenuFullInput", (void*)&data);
	EventSystem::GetInstance()->SendEventNow("CloseTowerMenu", nullptr);

	InputManager::s_CenterMouse = false;
	InputManager::ShowMouse(true);

	// === Get the Right Control Scheme
	if (InputManager::UsingController()) {
		m_pCloseBtnTexture->SetImage(Inferno::GetImage(L"UI_XboxBtn_Back.dds"));
		m_pNextBtnTexture->SetImage(Inferno::GetImage(L"UI_XboxBtn_RT.dds"));
		m_pPrevBtnTexture->SetImage(Inferno::GetImage(L"UI_XboxBtn_LT.dds"));
	}
	else {
		m_pCloseBtnTexture->SetImage(Inferno::GetImage(L"UI_KeyBtn_Esc.dds"));
		m_pNextBtnTexture->SetImage(Inferno::GetImage(L"UI_KeyBtn_E.dds"));
		m_pPrevBtnTexture->SetImage(Inferno::GetImage(L"UI_KeyBtn_Q.dds"));
	}
}
// ============================= //

// ===== Events ===== //
void HelpScreen::OnOpenHelpScreen(void* _pData)
{
	OpenScreen();
}

void HelpScreen::OnSetRecieveInput(void* _pData)
{
	bool recieveInput = *((bool*)_pData);
	if (recieveInput && !m_bRecieveInput) {
		m_bRecieveInput = true;

		using namespace InfernoEngine_Input;
		RegisterEventToActionBinding("Help", this, BindActionFunction(&HelpScreen::OnToggleHelpScreen, this));
	}
	else if (!recieveInput && m_bRecieveInput) {
		m_bRecieveInput = false;

		using namespace InfernoEngine_Input;
		UnregisterEventFromActionBinding("Help", this, BindActionFunction(&HelpScreen::OnToggleHelpScreen, this));
	}
}
// ================== //
