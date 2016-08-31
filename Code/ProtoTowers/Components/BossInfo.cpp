#include "stdafx.h"
#include "BossInfo.h"

#include "../InfernoUI.h"

// === Defines
#define HPBAR_WIDTH 32.0f
#define HPBAR_HEIGHT 256.0f

// ===== Constructor / Destructor ===== //
BossInfo::BossInfo(UIElement* _owner) : UIComponent(_owner)
{
	m_pHealthBarTexture = new Texture();
	m_pHealthBarTexture->SetImage(Inferno::GetImage(L"UI_Boss_HpBar.dds"));

	m_pLabelBackgroundTexture = new Texture();
	m_pLabelBackgroundTexture->SetImage(Inferno::GetImage(L"Fill.dds"));

	m_pHealthBar = nullptr;
	m_pBossLbl = nullptr;
}

BossInfo::~BossInfo()
{
	delete m_pHealthBarTexture;
	delete m_pLabelBackgroundTexture;
}
// ==================================== //

// ===== Interface ===== //
void BossInfo::OnEnable()
{
	if (m_pHealthBar == nullptr) {
		// === Set up the UIElement
		GetTransform()->SetAnchors(Anchors::ANCHOR_CENTERLEFT);
		GetTransform()->SetPosition(32.0f, 0.0f);
		GetTransform()->SetBounds(Vector2(HPBAR_WIDTH, HPBAR_HEIGHT));

		// === Create the Background 
		UIRenderer* background = this->GetUIElement()->AddComponent<UIRenderer>();
		background->SetTexture(m_pHealthBarTexture);
		background->SetColor(Color(0.5f, 0.5f, 0.5f, 1.0f));
		background->SetLayer(5);

		// === Create the Health Bar
		m_pHealthBar = this->GetUIElement()->AddComponent<UIRenderer>();
		m_pHealthBar->SetTexture(m_pHealthBarTexture);
		m_pHealthBar->SetColor(Color(1.0f, 0.1f, 0.1f, 1.0f));
		m_pHealthBar->SetEffect(UIEffects::UI_FILL_TOP_EFFECT);
		m_pHealthBar->SetImageFill(1.0f);
		m_pHealthBar->SetLayer(6);

		// === Create the Boss Label
		m_pLabelElement = new UIElement();
		m_pLabelElement->SetName("Boss Label");
		m_pLabelElement->GetTransform()->SetParent(this->GetTransform());
		m_pLabelElement->GetTransform()->SetAnchors(Anchors::ANCHOR_TOPCENTER);
		m_pLabelElement->GetTransform()->SetPosition((HPBAR_WIDTH / 2 + 2), -14.0f);
		m_pLabelElement->GetTransform()->SetBounds(Vector2(HPBAR_WIDTH * 2 + 4, 22));
		
		m_pBossLbl = m_pLabelElement->AddComponent<UIText>();
		m_pBossLbl->SetText("Boss");
		m_pBossLbl->SetFontSize(20.0f);
		m_pBossLbl->SetAlignment(TextAlignment::CENTER);
		m_pBossLbl->SetColor(Color(0.7f, 0.0f, 0.0f, 1.0f));
		m_pBossLbl->SetLayer(6);

		background = m_pLabelElement->AddComponent<UIRenderer>();
		background->SetTexture(m_pLabelBackgroundTexture);
		background->SetColor(Color(0.0f, 0.0f, 0.f, 1.0f));
		background->SetLayer(5);
	}
	else {
		m_pHealthBar->SetImageFill(1.0f);
	}
}
// ===================== //

// ===== Private Interface ===== //
void BossInfo::UpdateHealthBar(float _healthRatio)
{
	m_pHealthBar->SetImageFill(_healthRatio);
}
// ============================= //