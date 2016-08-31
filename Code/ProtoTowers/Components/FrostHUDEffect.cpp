#include "stdafx.h"
#include "FrostHUDEffect.h"

#include "../InfernoUI.h"
#include "EventSystem.h"

// === Defines 
#define GROWTH_SPEED 0.5f
#define TARGET_SIZE Vector2(1024.0f, 768.0f)
#define STARTING_SIZE (TARGET_SIZE * 1.2f)

// ===== Constructor / Destructor ===== //
FrostHUDEffect::FrostHUDEffect(UIElement* _owner) : UIComponent(_owner)
{
	m_pFrostImage = nullptr;
	m_v2GrowthSpeed = (STARTING_SIZE - TARGET_SIZE) * GROWTH_SPEED;
	m_uiEffectCounter = 0;
}

FrostHUDEffect::~FrostHUDEffect()
{
	OnDisable();
}
// ==================================== //

// ===== Interface ===== //
void FrostHUDEffect::OnEnable()
{
	if (m_pFrostImage == nullptr) {
		m_pFrostImage = GetUIElement()->GetComponent<UIRenderer>();
	}

	EventSystem::GetInstance()->RegisterForEvent("ToggleHUDFrost", this, BindEventFunction(&FrostHUDEffect::OnToggleFrost, this));
}

void FrostHUDEffect::OnDisable()
{
	EventSystem::GetInstance()->UnregisterFromEvent("ToggleHUDFrost", this, BindEventFunction(&FrostHUDEffect::OnToggleFrost, this));
}

void FrostHUDEffect::Update()
{
	if (m_pFrostImage == nullptr || !m_pFrostImage->IsEnabled()) {
		return;
	}

	Vector2 b = m_pRectTransform->GetBounds();
	if (m_pRectTransform->GetBounds().x * 2 > TARGET_SIZE.x) {
		Vector2 bounds = (m_pRectTransform->GetBounds() * 2) - TARGET_SIZE;
		float w = Clamp(0.0f, bounds.x, m_v2GrowthSpeed.x * Time::DeltaTime);
		float h = Clamp(0.0f, bounds.y, m_v2GrowthSpeed.y * Time::DeltaTime);

		bounds = m_pRectTransform->GetBounds() * 2;
		m_pRectTransform->SetBounds(Vector2(bounds.x - w, bounds.y - h));
	}
}
// ===================== //

// ===== Events ===== //
void FrostHUDEffect::OnToggleFrost(void* _pData)
{
	if (m_pFrostImage == nullptr) {
		return;
	}

	bool toggleFrost = *((bool*)_pData);

	if (toggleFrost) {
		if (m_uiEffectCounter == 0) {
			m_pFrostImage->SetEnabled(true);
			GetTransform()->SetBounds(STARTING_SIZE);
		}
		++m_uiEffectCounter;
	}
	else {
		--m_uiEffectCounter;
		if (m_uiEffectCounter == 0) {
			m_pFrostImage->SetEnabled(false);
		}
	}
}
// ================== //
