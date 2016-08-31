#include "stdafx.h"
#include "ResourceCounter.h"

#include "Player.h"
#include "UIElement.h"
#include "UIComponents\UIText.h"

// === Constructor / Destructor === //
ResourceCounter::ResourceCounter(UIElement* _owner) : UIComponent(_owner)
{
	m_pUIText = nullptr;
}

ResourceCounter::~ResourceCounter()
{

}
// ================================ //

// === Interface === //
void ResourceCounter::OnEnable()
{
	if (m_pUIText == nullptr) {
		m_pUIText = m_pUIElement->GetComponent<UIText>();
	}

	Player::AddForResourceTracking(this, BindForResourceTracking(&ResourceCounter::OnResourceUpdate, this));

	OnResourceUpdate(Player::GetResources());
}

void ResourceCounter::OnDisable()
{
	Player::RemoveFromResourceTracking(this);
}

void ResourceCounter::OnResourceUpdate(unsigned int _newAmount)
{
 	if (m_pUIText != nullptr) {
		m_pUIText->SetText(std::to_string(_newAmount));
	}
}
// ================= //
