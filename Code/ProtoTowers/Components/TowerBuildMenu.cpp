#include "stdafx.h"
#include "TowerBuildMenu.h"

#include "../InfernoUI.h"
#include "BaseTower.h"
#include "EventSystem.h"
#include "InputManager.h"
#include "ModuleInterface.h"
#include "TowerDescription.h"
#include "TowerNode.h"

// === Defines
#define TOWER_DESC_XOFFSET 65.0f

// ===== Constructor / Destructor ===== //
TowerBuildMenu::TowerBuildMenu(UIElement* _owner) : UIComponent(_owner)
{
	m_pTowerDescription = nullptr;
	m_pOwningNode = nullptr;
	m_pSelectImage = nullptr;
	m_pBackImage = nullptr;

	m_pSelectBtnTexture = new Texture();
	m_pSelectBtnTexture->SetImage(Inferno::GetImage(L"UI_XboxBtn_A.dds"));
	m_pBackBtnTexture = new Texture();
	m_pBackBtnTexture->SetImage(Inferno::GetImage(L"UI_XboxBtn_B.dds"));
}

TowerBuildMenu::~TowerBuildMenu()
{
	delete m_pSelectBtnTexture;
	delete m_pBackBtnTexture;
}
// ==================================== //

// ===== Interface ===== //
void TowerBuildMenu::PostInit()
{
	if (m_pSelectImage == nullptr) {
		UIElement* controlsInfo = GetTransform()->GetChildByName(std::string("BM Controls Info"), true);
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
		transform->SetPosition(-135.0f, 0.0f);
		transform->SetBounds(Vector2(48.0f, 48.0f));
		//// Components
		// == UIRenderer (Select Img)
		image = selectImg->AddComponent<UIRenderer>();
		image->SetTexture(m_pSelectBtnTexture);
		image->SetColor(Color(0.8f, 0.8f, 0.8f, 1.0f));

		// === Back Btn Image
		UIElement* backImg = new UIElement();
		transform = backImg->GetTransform();
		transform->SetParent(controlsInfo->GetTransform());
		transform->SetPosition(12.0f, 0.0f);
		transform->SetBounds(Vector2(48.0f, 48.0f));
		//// Components
		// == UIRenderer (Back Img)
		image = backImg->AddComponent<UIRenderer>();
		image->SetTexture(m_pBackBtnTexture);
		image->SetColor(Color(0.8f, 0.8f, 0.8f, 1.0f));
#pragma endregion
	}
}

void TowerBuildMenu::OnEnable()
{
	using namespace InfernoEngine_Input;
	RegisterEventToActionBinding("SecondaryInteract", this, BindActionFunction(&TowerBuildMenu::OnMenuExit, this));
	RegisterEventToActionBinding("Escape", this, BindActionFunction(&TowerBuildMenu::OnMenuExit, this));

	Inferno_UI::SetCurrentSelectable(m_pFirstSelectable);

	bool data = false;
	EventSystem* pEventSystem = EventSystem::GetInstance();
	pEventSystem->SendEventNow("TogglePauseMenuInput", (void*)&data);
	pEventSystem->SendEventNow("PlayerRecieveInput", (void*)&data);
	pEventSystem->SendEventNow("ShowCrosshair", (void*)&data);
	pEventSystem->RegisterForEvent("CloseTowerMenu", this, BindEventFunction(&TowerBuildMenu::OnCloseMenu, this));

	InputManager::s_CenterMouse = false;
	InputManager::ShowMouse(true);

	UIElement* foundElement;
	// === Get the Tower Description
	if (m_pTowerDescription == nullptr) {
		foundElement = m_pRectTransform->GetChildByName(std::string("Tower Description Element"));
		if (foundElement != nullptr) {
			m_pTowerDescription = foundElement->GetComponent<TowerDescription>();

			m_pTowerDescription->GetUIElement()->SetActive(false);
		}
	}

	// === Get the Turret Btn Positions
	foundElement = m_pRectTransform->GetChildByName(std::string("AutoTurret Btn"));
	if (foundElement != nullptr) {
		m_pButtonPositions[(unsigned int)Towers::AutoTurret] = foundElement->GetTransform()->GetPosition();
	}
	foundElement = m_pRectTransform->GetChildByName(std::string("FreezeTower Btn"));
	if (foundElement != nullptr) {
		m_pButtonPositions[(unsigned int)Towers::FreezeTower] = foundElement->GetTransform()->GetPosition();
	}
	foundElement = m_pRectTransform->GetChildByName(std::string("Mortar Btn"));
	if (foundElement != nullptr) {
		m_pButtonPositions[(unsigned int)Towers::Mortar] = foundElement->GetTransform()->GetPosition();
	}
	foundElement = m_pRectTransform->GetChildByName(std::string("PoisonTurret Btn"));
	if (foundElement != nullptr) {
		m_pButtonPositions[(unsigned int)Towers::PoisonTurret] = foundElement->GetTransform()->GetPosition();
	}

	// === Get the right Control Scheme
	if (InputManager::UsingController()) {
		m_pSelectBtnTexture->SetImage(Inferno::GetImage(L"UI_XboxBtn_A.dds"));
		m_pBackBtnTexture->SetImage(Inferno::GetImage(L"UI_XboxBtn_B.dds"));
	}
	else {
		m_pSelectBtnTexture->SetImage(Inferno::GetImage(L"UI_KeyBtn_E.dds"));
		m_pBackBtnTexture->SetImage(Inferno::GetImage(L"UI_KeyBtn_Esc.dds"));
	}

	Time::TimeScale = 0.25f;
}

void TowerBuildMenu::OnDisable()
{
	using namespace InfernoEngine_Input;
	UnregisterEventFromActionBinding("SecondaryInteract", this, BindActionFunction(&TowerBuildMenu::OnMenuExit, this));
	UnregisterEventFromActionBinding("Escape", this, BindActionFunction(&TowerBuildMenu::OnMenuExit, this));

	Inferno_UI::SetCurrentSelectable(nullptr);

	bool data = true;
	EventSystem* pEventSystem = EventSystem::GetInstance();
	pEventSystem->SendEventNow("TogglePauseMenuInput", (void*)&data);
	pEventSystem->SendEventNow("PlayerRecieveInput", (void*)&data);
	pEventSystem->SendEventNow("ShowCrosshair", (void*)&data);
	pEventSystem->UnregisterFromEvent("CloseTowerMenu", this, BindEventFunction(&TowerBuildMenu::OnCloseMenu, this));

	Time::TimeScale = 1.0f;

#if !defined(_DEBUG) 
	InputManager::s_CenterMouse = true;
	InputManager::ShowMouse(false);
#endif
}
// ===================== //

// ===== Private Interface ===== //
void TowerBuildMenu::CloseMenu()
{
	Inferno_UI::SetCurrentSelectable(nullptr);

	bool data = true;
	EventSystem::GetInstance()->SendEventNow("PlayerRecieveInput", (void*)&data);

#if !defined(_DEBUG) 
	InputManager::s_CenterMouse = true;
	InputManager::ShowMouse(false);
#endif

	Time::TimeScale = 1.0f;

	m_pOwningNode->CloseMenu();
}
// ============================= //

// ===== Events ===== //
void TowerBuildMenu::OnCloseMenu(void* _pData)
{
	CloseMenu();
}
// ================== //

// ===== Button Functions ===== //
void TowerBuildMenu::OnTurretTabClick()
{
	
}

void TowerBuildMenu::OnBuildingsTabClick()
{
	
}

void TowerBuildMenu::OnAutoTurretBtnClick()
{
	if (Player::GetResources() < 100) {
		CloseMenu();
	}
	if (m_pOwningNode != nullptr && Player::UseResources(BaseTower::GetPriceOfTower(TowerTypes::TT_AUTOTURRET))) {
		m_pOwningNode->BuildTower(TowerTypes::TT_AUTOTURRET);
		CloseMenu();
	}
}

void TowerBuildMenu::OnAutoTurretHover()
{
	Vector2 position = m_pButtonPositions[(unsigned int)Towers::AutoTurret];
	position.x += TOWER_DESC_XOFFSET;
	m_pTowerDescription->DisplayDescription(std::string("Basic Machine Gun Turret. Medium Damage, High FireRate."), position);

	m_pTowerDescription->GetUIElement()->SetActive(true);
}

void TowerBuildMenu::OnFreezeTowerBtnClick()
{
	if (Player::GetResources() < 100) {
		CloseMenu();
	}

	if (m_pOwningNode != nullptr && Player::UseResources(BaseTower::GetPriceOfTower(TowerTypes::TT_FREEZETOWER))) {
		m_pOwningNode->BuildTower(TowerTypes::TT_FREEZETOWER);
		CloseMenu();
	}
}

void TowerBuildMenu::OnFreezeTurretHover()
{
	Vector2 position = m_pButtonPositions[(unsigned int)Towers::FreezeTower];
	position.x += TOWER_DESC_XOFFSET;
	m_pTowerDescription->DisplayDescription(std::string("Slows down enemies when they get close and deals a small amount of damage over time."), position);

	m_pTowerDescription->GetUIElement()->SetActive(true);
}

void TowerBuildMenu::OnMortarBtnClick()
{
	if (Player::GetResources() < 100) {
		CloseMenu();
	}
	
	if (m_pOwningNode != nullptr && Player::UseResources(BaseTower::GetPriceOfTower(TowerTypes::TT_MORTAR))) {
		m_pOwningNode->BuildTower(TowerTypes::TT_MORTAR);
		CloseMenu();
	}
}

void TowerBuildMenu::OnMortarTurretHover()
{
	Vector2 position = m_pButtonPositions[(unsigned int)Towers::Mortar];
	position.x += TOWER_DESC_XOFFSET;
	m_pTowerDescription->DisplayDescription(std::string("Artillery Turret. High Damage, AOE, Low FireRate."), position);

	m_pTowerDescription->GetUIElement()->SetActive(true);
}

void TowerBuildMenu::OnPoisonTurretClick()
{
	if (Player::GetResources() < 100) {
		CloseMenu();
	}
	
	if (m_pOwningNode != nullptr && Player::UseResources(BaseTower::GetPriceOfTower(TowerTypes::TT_POISONTOWER))) {
		m_pOwningNode->BuildTower(TowerTypes::TT_POISONTOWER);
		CloseMenu();
	}
}

void TowerBuildMenu::OnPoisonTurretHover()
{
	Vector2 position = m_pButtonPositions[(unsigned int)Towers::PoisonTurret];
	position.x += TOWER_DESC_XOFFSET;
	m_pTowerDescription->DisplayDescription(std::string("Shoots a constant stream of toxin, damaging any enemies caught in it for a few seconds. "), position);

	m_pTowerDescription->GetUIElement()->SetActive(true);
}

void TowerBuildMenu::OnTowerBtnHoverEnd()
{
	m_pTowerDescription->GetUIElement()->SetActive(false);
}
// ============================ //

// ===== Text Functions ===== //
void TowerBuildMenu::OnResourcesUpdate(unsigned int _newAmount)
{
	if (m_pResourcesText != nullptr) {
		m_pResourcesText->SetText(std::to_string(_newAmount));
	}
}
// ========================== //

// ===== Input Functions ===== //
void TowerBuildMenu::OnMenuExit(ACTION_STATUS _status)
{
	CloseMenu();
}
// =========================== //

// === Mutators === //
void TowerBuildMenu::SetOwningNode(TowerNode* _node)
{
	m_pOwningNode = _node;
}

void TowerBuildMenu::SetFirstSelectable(UISelectable* _selectable)
{
	m_pFirstSelectable = _selectable;
}
// ================ //