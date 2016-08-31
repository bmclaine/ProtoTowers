#include "../Public/UIComponents/UIButton.h"

#include "../Public/UIComponents/UIEventSystem.h"
#include "../Public/UIComponents/UIRenderer.h"
#include "../Public/UIElement.h"
#include "AudioManager.h"

// === Constructor / Destructor === //
UIButton::UIButton(UIElement* _owner) : UISelectable(_owner)
{
	m_ImageDisabled.Texture = nullptr;
	m_ImageHover.Texture = nullptr;
	m_ImageNormal.Texture = nullptr;
	m_ImagePressed.Texture = nullptr;
	m_pImage = m_pUIElement->AddComponent<UIRenderer>();
}

UIButton::~UIButton()
{

}
// ================================ //

// ===== Interface ===== //

// === PostInit
//  IN : void
//  OUT : void
//
//  Handles Registering all the functions to the EventSystem that is on the UIElement, and calls it's parent PostInit function.
void UIButton::PostInit()
{
	UIEventSystem* eventSystem = UISelectable::m_pUIElement->GetEventSystem();
	eventSystem->RegisterEvent(UI_EVENTS::UI_EVENT_ONDISABLED, BindFunction(&UIButton::OnDisabled));
	eventSystem->RegisterEvent(UI_EVENTS::UI_EVENT_ONENABLED, BindFunction(&UIButton::OnEnabled));
	eventSystem->RegisterEvent(UI_EVENTS::UI_EVENT_ONHOVERBEGIN, BindFunction(&UIButton::OnSelect));
	eventSystem->RegisterEvent(UI_EVENTS::UI_EVENT_ONHOVEREND, BindFunction(&UIButton::OnEnabled));
	eventSystem->RegisterEvent(UI_EVENTS::UI_EVENT_ONCLICK, BindFunction(&UIButton::OnPressed));
	eventSystem->RegisterEvent(UI_EVENTS::UI_EVENT_ONRELEASED, BindFunction(&UIButton::OnEnabled));

	OnEnabled();

	UISelectable::PostInit();
}
// ===================== //

// ===== Event Functions ===== //

// === OnSelect
//  IN : void
//  OUT : void
//
//  Event Function that is triggered when the Button is Selected / Hovered over
void UIButton::OnSelect()
{
	if (m_ImageHover.Texture != nullptr) {
		m_pImage->SetColor(m_ImageHover.ColorOffset);
		m_pImage->SetTexture(m_ImageHover.Texture);
	}

	AudioManager::Instance()->PostEvent(m_uiSelectSound);
}

// === OnDeselect
//  IN : void
//  OUT : void
//
//  Event Function that is triggered when the Button is Deselected / done being hovered over
void UIButton::OnDeselect()
{
	if (m_ImageNormal.Texture != nullptr) {
		m_pImage->SetColor(m_ImageNormal.ColorOffset);
		m_pImage->SetTexture(m_ImageNormal.Texture);
	}
}

// === OnPressed
//  IN : void
//  OUT : void
//
//  Event Function that is triggered when the Button is clicked 
void UIButton::OnPressed()
{
	if (m_ImagePressed.Texture != nullptr) {
		m_pImage->SetColor(m_ImagePressed.ColorOffset);
		m_pImage->SetTexture(m_ImagePressed.Texture);
	}

	//AudioManager::Instance()->PostEvent(m_uiPressedSound);
}

// === OnDisabled
//  IN : void
//  OUT : void
//
//  Event Function that is triggered when the Button is disabled
void UIButton::OnDisabled()
{
	if (m_ImageDisabled.Texture != nullptr) {
		m_pImage->SetColor(m_ImageDisabled.ColorOffset);
		m_pImage->SetTexture(m_ImageDisabled.Texture);
	}
}

// === OnEnabled
//  IN : void
//  OUT : void
//
//  Event Function that is triggered when the Button is Enabled or switching back to a 'Normal' state
void UIButton::OnEnabled()
{
	if (m_ImageNormal.Texture != nullptr) {
		m_pImage->SetColor(m_ImageNormal.ColorOffset);
		m_pImage->SetTexture(m_ImageNormal.Texture);
	}
}
// =========================== //

// === Mutators === //
void UIButton::SetNormalImage(Texture* _texture, Color _colorOffset)
{
	m_ImageNormal = { _colorOffset, _texture };
}

void UIButton::SetHoverImage(Texture* _texture, Color _colorOffset)
{
	m_ImageHover = { _colorOffset, _texture };
}

void UIButton::SetPressedImage(Texture* _texture, Color _colorOffset)
{
	m_ImagePressed = { _colorOffset, _texture };
}

void UIButton::SetDisabledImage(Texture* _texture, Color _colorOffset)
{
	m_ImageDisabled = { _colorOffset, _texture };
}

void UIButton::SetLayer(unsigned int _layer)
{
	m_pImage->SetLayer(_layer);
}
// ================ //