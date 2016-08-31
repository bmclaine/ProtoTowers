#include "stdafx.h"
#include "TowerInfo.h"

#include "../InfernoUI.h"
#include "BaseTower.h"

// ===== Constructor / Destructor ===== //
TowerInfo::TowerInfo(UIElement* _owner) : UIComponent(_owner), m_fcTowerInfoFlags((unsigned int)TI_MAX_FLAGS)
{
	m_eType = TowerTypes::TT_UNKNOWN;
	m_fNameXOffset = 0.0f;
	m_fNameYOffset = -30.0f;
	m_fUnavailXOffset = m_fUnavailYOffset = 0.0f;
	m_fPriceXOffset = 0.0f;
	m_fPriceYOffset = 70.0f;
	m_fcTowerInfoFlags.SetFlag((unsigned int)TI_CanBuy_Flag, (unsigned int)true);

	m_pUnavailableTexture = new Texture();
	m_pUnavailableTexture->SetImage(Inferno::GetImage(L"UI_xOut_Img.dds"));

	// === Name Element
	RectTransform* transform;
	UIElement* nameElement = new UIElement();
	transform = nameElement->GetTransform();
	transform->SetParent(GetTransform());
	transform->SetPosition(0.0f, m_fNameYOffset);
	transform->SetBounds(Vector2(120.0f, 25.0f));
	//// Components
		m_pName = nameElement->AddComponent<UIText>();
		m_pName->SetLayer(6);
		m_pName->SetAlignment(TextAlignment::CENTER);

	// === Unavailable Element
	UIElement* unavailElement = new UIElement();
	transform = unavailElement->GetTransform();
	transform->SetParent(GetTransform());
	transform->SetPosition(m_fUnavailXOffset, m_fUnavailYOffset);
	transform->SetBounds(Vector2(64.0f, 64.0f));
	//// Components
		m_pUnavailableImg = unavailElement->AddComponent<UIRenderer>();
		m_pUnavailableImg->SetLayer(5);
		m_pUnavailableImg->SetTexture(m_pUnavailableTexture);
		m_pUnavailableImg->SetEnabled(false);

	// === Price Element
	UIElement* priceElement = new UIElement();
	transform = priceElement->GetTransform();
	transform->SetParent(GetTransform());
	transform->SetPosition(0.0f, m_fPriceYOffset);
	transform->SetBounds(Vector2(100.0f, 25.0f));
	//// Components 
		m_pPrice = priceElement->AddComponent<UIText>();
		m_pPrice->SetLayer(6);
		m_pPrice->SetAlignment(TextAlignment::CENTER);
}

TowerInfo::~TowerInfo()
{
	delete m_pUnavailableTexture;
}
// ==================================== // 

// ===== Interface ===== //
void TowerInfo::OnEnable()
{
	m_uiTowerCost = BaseTower::GetPriceOfTower(m_eType);

	m_pName->GetTransform()->SetPosition(m_fNameXOffset, m_fNameYOffset);
	m_pName->SetText(BaseTower::GetNameOfTower(m_eType));
	m_pName->SetColor(Color(0.0f, 1.0f, 0.0f, 1.0f));
	m_pName->SetFontSize(18.0f);

	m_pUnavailableImg->GetTransform()->SetPosition(m_fUnavailXOffset, m_fUnavailYOffset);

	m_pPrice->GetTransform()->SetPosition(m_fPriceXOffset, m_fPriceYOffset);
	m_pPrice->SetText(std::to_string(m_uiTowerCost));
	m_pPrice->SetColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
	m_pPrice->SetFontSize(22.0f);

	Player::AddForResourceTracking(this, BindForResourceTracking(&TowerInfo::OnResourceUpdate, this));

	if (!m_fcTowerInfoFlags.GetFlag((unsigned int)TI_Unavailable_Flag)) {
		m_fcTowerInfoFlags.SetFlag((unsigned int)TI_CanBuy_Flag, (unsigned int)true);
		m_pUnavailableImg->SetEnabled(false);
	}
	OnResourceUpdate(Player::GetResources());
}

void TowerInfo::OnDisable()
{
	Player::RemoveFromResourceTracking(this);
}
// ===================== //

// ===== Private Interface ===== //
void TowerInfo::OnResourceUpdate(unsigned int _newAmount)
{
	if (m_fcTowerInfoFlags.GetFlag((unsigned int)TI_Unavailable_Flag)) {
		return;
	}

	if (_newAmount < m_uiTowerCost && m_fcTowerInfoFlags.GetFlag((unsigned int)TI_CanBuy_Flag)) {
		// === Not Enough Resources
		m_pPrice->SetColor(Color(1.0f, 0.0f, 0.0f, 1.0f));
		m_pUnavailableImg->SetEnabled(true);
		m_fcTowerInfoFlags.SetFlag((unsigned int)TI_CanBuy_Flag, (unsigned int)false);
	}
	else if (_newAmount >= m_uiTowerCost && !m_fcTowerInfoFlags.GetFlag((unsigned int)TI_CanBuy_Flag)) {
		// === Has Enough Resources
		m_pPrice->SetColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
		m_pUnavailableImg->SetEnabled(false);
		m_fcTowerInfoFlags.SetFlag((unsigned int)TI_CanBuy_Flag, (unsigned int)true);
	}
}
// ============================= //

// ===== Mutators ===== //
void TowerInfo::SetType(TowerTypes _type)
{
	m_eType = _type;
}

void TowerInfo::SetNameOffset(float _xOffset, float _yOffset)
{
	m_fNameXOffset = _xOffset;
	m_fNameYOffset = _yOffset;
}

void TowerInfo::SetUnavailableOffset(float _xOffset, float _yOffset)
{
	m_fUnavailXOffset = _xOffset;
	m_fUnavailYOffset = _yOffset;
}

void TowerInfo::SetPriceOffset(float _xOffset, float _yOffset)
{
	m_fPriceXOffset = _xOffset;
	m_fPriceYOffset = _yOffset;
}

void TowerInfo::SetAsUnavailable(bool _value)
{
	m_pUnavailableImg->SetEnabled(_value);
	m_fcTowerInfoFlags.SetFlag((unsigned int)TI_Unavailable_Flag, (unsigned int)_value);
}
// ==================== //
