#include "UIComponents\UISelectable.h"

#include "InputEvents.h"
#include "UIComponents\UIComponent.h"
#include "UIComponents\UIEventSystem.h"
#include "UIElement.h"
#include "WWiseResources\Wwise_IDs.h"

// ===== Constructor / Destructor ===== //
UISelectable::UISelectable(UIElement* _owner) : UIComponent(_owner)
{
	m_pUpSelectable = nullptr;
	m_pDownSelectable = nullptr;
	m_pRightSelectable = nullptr;
	m_pLeftSelectable = nullptr;
	m_uiPressedSound = AK::EVENTS::UI_PRESS_BUTTON;
	m_uiSelectSound = AK::EVENTS::UI_PRESS_BUTTON;
	m_bIsDisabled = false;
}

UISelectable::~UISelectable()
{

}
// ==================================== //

// ===== Interface ===== //
void UISelectable::PostInit()
{
	UIEventSystem* eventSystem = m_pUIElement->GetEventSystem();
	eventSystem->RegisterEvent(UI_EVENTS::UI_EVENT_ONSELECT, BindFunction(&UISelectable::OnSelect));  // Might need to be passed as a reference
	eventSystem->RegisterEvent(UI_EVENTS::UI_EVENT_ONDESELECT, BindFunction(&UISelectable::OnDeselect));
	eventSystem->RegisterForMouseHover(BindMouseMovementFunction(&UISelectable::OnMouseOver, this));
}
// ===================== //

// === Event Functions === //
void UISelectable::OnMouseOver(tagPOINT& _cursorPosition, tagPOINT& _displacement)
{
	
}
// ======================= //

// === Accessors === //
UISelectable* UISelectable::GetUpSelectable() const
{
	return m_pUpSelectable;
}

UISelectable* UISelectable::GetDownSelectable() const
{
	return m_pDownSelectable;
}

UISelectable* UISelectable::GetRightSelectable() const
{
	return m_pRightSelectable;
}

UISelectable* UISelectable::GetLeftSelectable() const
{
	return m_pLeftSelectable;
}


bool UISelectable::GetDisabled() const
{
	return m_bIsDisabled;
}
// ================= //

// === Mutators === //
void UISelectable::SetUpSelectable(UISelectable* _selectable)
{
	m_pUpSelectable = _selectable;
}

void UISelectable::SetDownSelectable(UISelectable* _selectable)
{
	m_pDownSelectable = _selectable;
}

void UISelectable::SetRightSelectable(UISelectable* _selectable)
{
	m_pRightSelectable = _selectable;
}

void UISelectable::SetLeftSelectable(UISelectable* _selectable)
{
	m_pLeftSelectable = _selectable;
}

void UISelectable::SetPressedSound(unsigned int _soundID)
{
	m_uiPressedSound = _soundID;
}

void UISelectable::SetSelectSound(unsigned int _soundID)
{
	m_uiSelectSound = _soundID;
}
void UISelectable::SetDisabled(bool disabled)
{
	m_bIsDisabled = disabled;

	if (disabled)
		m_pUIElement->GetEventSystem()->InvokeEvent(UI_EVENTS::UI_EVENT_ONDISABLED);
	else
		m_pUIElement->GetEventSystem()->InvokeEvent(UI_EVENTS::UI_EVENT_ONENABLED);
}
// ================ //