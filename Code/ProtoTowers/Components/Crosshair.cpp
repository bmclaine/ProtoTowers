#include "stdafx.h"
#include "Crosshair.h"

#include "../InfernoUI.h"
#include "EventSystem.h"

// === Constructor / Destructor === //
Crosshair::Crosshair(UIElement* _owner) : UIComponent(_owner)
{
	m_pImage = nullptr;
}

Crosshair::~Crosshair()
{

}
// ================================ //

// === Interface === //
void Crosshair::OnEnable()
{
	if (m_pImage == nullptr) {
		m_pImage = m_pUIElement->GetComponent<UIRenderer>();
	}

	EventSystem::GetInstance()->GetInstance()->RegisterForEvent("ShowCrosshair", this, BindEventFunction(&Crosshair::OnShowCrosshair, this));
}

void Crosshair::OnDisable()
{
	EventSystem::GetInstance()->GetInstance()->UnregisterFromEvent("ShowCrosshair", this, BindEventFunction(&Crosshair::OnShowCrosshair, this));
}
// ================= //

// === Events === //
void Crosshair::OnShowCrosshair(void* _pData)
{
	bool show = *(static_cast<bool*>(_pData));
	m_pImage->SetEnabled(show);
}
// ============== //