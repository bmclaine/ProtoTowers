#include "stdafx.h"
#include "TowerOptionsScript.h"

#include "../InfernoUI.h"
#include "BaseTower.h"
#include "EventSystem.h"
#include "InputManager.h"
#include "Player.h"
#include "TowerNode.h"

// === Defines
#define HEIGHT_OFFSET -46.0f
#define SPACER 4.0f
#define TEXT_SIZE 22.0f
#define TEXT_OFFSET -26.0f

// ===== Constructor / Destructor ===== //
TowerOptionsScript::TowerOptionsScript(UIElement* _owner) : UIComponent(_owner)
{
	m_pFocusedTower = nullptr;

	m_pUpgradeIcon = new Texture();
	m_pSellIcon = new Texture();
	m_pRepairIcon = new Texture();

	m_pUpgradeControlIcon = nullptr;
	m_pSellControlIcon = nullptr;
	m_pRepairControlIcon = nullptr;

	m_pUpgradeText = nullptr;
	m_pSellText = nullptr;
	m_pRepairText = nullptr;

	m_iRepairPrice = m_iSellPrice = m_iUpgradePrice = 0;
	m_bUpgrade = false;
}

TowerOptionsScript::~TowerOptionsScript()
{
	delete m_pUpgradeIcon;
	delete m_pSellIcon;
	delete m_pRepairIcon;
}
// ==================================== //

// ===== Interface ===== //
void TowerOptionsScript::OnEnable()
{
	// === Register for Input Type Handling
	InputManager::RegisterEventToControlChange(this, BindControlChangeFunction(&TowerOptionsScript::UpdateControlIcons, this));
	InputManager::RegisterEventToActionBinding("Interact", this, BindActionFunction(&TowerOptionsScript::OnRepairBtn, this));
	InputManager::RegisterEventToActionBinding("SecondaryInteract", this, BindActionFunction(&TowerOptionsScript::OnSellBtn, this));

	UpdateControlIcons(InputManager::UsingController());

	// === Have we created the UIElements ye?
	if (m_pSellControlIcon == nullptr) {
		CreateUIElements();
	}

	if (m_pFocusedTower != nullptr) {
		UpdatePrices(m_pFocusedTower->GetHealth(), m_pFocusedTower->GetMaxHealth());
	}
}

void TowerOptionsScript::OnDisable()
{
	// === Unregister from Input Type Handling
	InputManager::UnregisterEventFromControlChange(this, BindControlChangeFunction(&TowerOptionsScript::UpdateControlIcons, this));
	InputManager::UnregisterEventFromAction("Interact", this, BindActionFunction(&TowerOptionsScript::OnRepairBtn, this));
	InputManager::UnregisterEventFromAction("SecondaryInteract", this, BindActionFunction(&TowerOptionsScript::OnSellBtn, this));

	m_pFocusedTower = nullptr;

	SetupForMinionSpawner(false);
}

void TowerOptionsScript::UpdatePrices(float _health, float _maxHealth)
{
	float rhRatio = _health / _maxHealth;
	float lhRatio = (_maxHealth - _health) / _maxHealth;
	unsigned int towerPrice = BaseTower::GetPriceOfTower(m_pFocusedTower->GetType());

	// === Sell Price : 75% of Tower Buy Price * rhRatio
	m_iSellPrice = (int)((towerPrice * 0.75f) * rhRatio);
	std::string text = "Sell ("; text += std::to_string(m_iSellPrice); text += ")";
	m_pSellText->SetText(text);

	m_bUpgrade = false;
	// === Repair Price : 75% of Tower Buy Price * lhRatio 
	if (_health != _maxHealth) {
		m_iRepairPrice = (int)((towerPrice * 0.75f) * lhRatio);
		text = "Repair ("; text += std::to_string(m_iRepairPrice); text += ")";
	}
//	else if (m_pFocusedTower->GetType() == TowerTypes::TT_SPAWNER) {
//		text = "Repair (Full)";
//	}
	else {
		if (m_pFocusedTower->GetUpgradeLevel() < 2) {
			m_iUpgradePrice = BaseTower::GetUpgradePrice(m_pFocusedTower->GetType());
			text = "Upgrade ("; text += std::to_string(m_iUpgradePrice); text += ")";
			m_bUpgrade = true;
			m_bUpgradeAvailable = true;
		}
		else {
			text = "Upgrade (N/A)";
			m_bUpgrade = true;
			m_bUpgradeAvailable = false;
		}
	}

	m_pRepairText->SetText(text);
}

void TowerOptionsScript::SetupForMinionSpawner(bool _value)
{
	if (_value) {
		// === Turn off the Sell Display / Option
		m_pSellControlIcon->GetUIElement()->SetActive(false);
		m_pSellText->GetUIElement()->SetActive(false);
	}
	else {
		// === Turn off the Sell Display / Option
		m_pSellControlIcon->GetUIElement()->SetActive(true);
		m_pSellText->GetUIElement()->SetActive(true);

		// TODO : Revert the changes when Minion choosing is implemented
	}
}
// ===================== //

// ===== Private Interface ===== //
void TowerOptionsScript::CreateUIElements()
{
	RectTransform* transform;
	UIElement* element;

	// TODO : Add Upgrade UI Stuff ...

	// === Repair Control Icon
	element = new UIElement();
	element->SetName("TO Repair Control Icon");
	transform = element->GetTransform();
	transform->SetParent(GetTransform());
	transform->SetAnchors(Anchors::ANCHOR_BOTTOMCENTER);
	transform->SetPosition(0.0f, HEIGHT_OFFSET - 32.0f);
	transform->SetBounds(Vector2(54.0f, 54.0f));
	//// Components
		// == UIRenderer
		m_pRepairControlIcon = element->AddComponent<UIRenderer>();
		m_pRepairControlIcon->SetTexture(m_pRepairIcon);

	// === Repair Text
	element = new UIElement();
	element->SetName("TO Repair Text");
	transform = element->GetTransform();
	transform->SetParent(GetTransform());
	transform->SetAnchors(Anchors::ANCHOR_BOTTOMCENTER);
	transform->SetPosition(32.0f + SPACER + 64.0f, HEIGHT_OFFSET + TEXT_OFFSET);
	transform->SetBounds(Vector2(156.0f, 40.0f));
	//// Components
		// == UIText
		m_pRepairText = element->AddComponent<UIText>();
		m_pRepairText->SetFontSize(TEXT_SIZE);

	// === Sell Control Icon
	element = new UIElement();
	element->SetName("TO Sell Control Icon");
	transform = element->GetTransform();
	transform->SetParent(GetTransform());
	transform->SetAnchors(Anchors::ANCHOR_BOTTOMCENTER);
	transform->SetPosition(200.0f, HEIGHT_OFFSET - 32.0f);
	transform->SetBounds(Vector2(54.0f, 54.0f));
	//// Components
		// == UIRenderer
		m_pSellControlIcon = element->AddComponent<UIRenderer>();
		m_pSellControlIcon->SetTexture(m_pSellIcon);

	// === Sell Text
	element = new UIElement();
	element->SetName("TO Sell Text");
	transform = element->GetTransform();
	transform->SetParent(GetTransform());
	transform->SetAnchors(Anchors::ANCHOR_BOTTOMCENTER);
	transform->SetPosition(200.0f + 32.0f + SPACER + 64.0f, HEIGHT_OFFSET + TEXT_OFFSET);
	transform->SetBounds(Vector2(128.0f, 40.0f));
	//// Components
		// == UIText
		m_pSellText = element->AddComponent<UIText>();
		m_pSellText->SetFontSize(TEXT_SIZE);

}

void TowerOptionsScript::UpdateControlIcons(bool _controller)
{
	if (_controller) {
		m_pUpgradeIcon->SetImage(Inferno::GetImage(L"UI_XboxBtn_X.dds"));
		m_pSellIcon->SetImage(Inferno::GetImage(L"UI_XboxBtn_B.dds"));
		m_pRepairIcon->SetImage(Inferno::GetImage(L"UI_XboxBtn_A.dds"));
	}
	else {
		m_pUpgradeIcon->SetImage(Inferno::GetImage(L"UI_KeyBtn_E.dds"));
		m_pSellIcon->SetImage(Inferno::GetImage(L"UI_KeyBtn_Q.dds"));
		m_pRepairIcon->SetImage(Inferno::GetImage(L"UI_KeyBtn_E.dds"));
	}
}

void TowerOptionsScript::RepairTower()
{
	if (Player::UseResources(m_iRepairPrice)) {
		m_pFocusedTower->Repair();
	}
	else {
		// === Audio (Failure to Repair)
		Inferno::PostAudioEvent(WWiseIDs::NOT_ENOUGH_RESOURCES);
	}
}

void TowerOptionsScript::SellTower()
{
	Player::AddResources(m_iSellPrice);

	m_pFocusedTower->Sell();
}

void TowerOptionsScript::UpgradeTower()
{
	struct EventData {
		unsigned int Team;
		unsigned int Type;
	};

	if (!m_bUpgradeAvailable) {
		return;
	}

	if (Player::UseResources(m_iUpgradePrice)) {
		if (m_pFocusedTower->GetType() == TowerTypes::TT_SPAWNER) {
			EventData data = { (unsigned int)Team::BLUE, (unsigned int)TowerTypes::TT_SPAWNER };
			EventSystem::GetInstance()->SendEventNow("UpgradeTowers", &data);
		}
		else {
			TowerNode::UpgradeTowerType(m_pFocusedTower->GetType());
		}

		// === Play Audio
		Inferno::PostAudioEvent(WWiseIDs::DEFAULT_TURRET_UPGRADE);

		this->GetUIElement()->SetActive(false);
	}
	else {
		// === Audio (Not Enough Funds)
		Inferno::PostAudioEvent(WWiseIDs::NOT_ENOUGH_RESOURCES);
	}
}
// ============================= //

// ===== Input Events ===== //
void TowerOptionsScript::OnRepairBtn(ACTION_STATUS _status)
{
	if (_status == ACTION_STATUS::AS_PRESSED) {
		if (m_bUpgrade) {
			UpgradeTower();
		}
		else {
			RepairTower();
		}
	}
}

void TowerOptionsScript::OnSellBtn(ACTION_STATUS _status)
{
	if (_status == ACTION_STATUS::AS_PRESSED) {
		SellTower();
	}
}
// ======================== //

// ===== Mutators ===== //
void TowerOptionsScript::SetFocusedTower(BaseTower* _tower)
{
	m_pFocusedTower = _tower;
}
// ==================== //
