#include "UIComponents\UIEventSystem.h"

#include "../../Interface/Public/ModuleInterface.h"

#include <Windows.h>

// === TEMP
#include "UIElement.h"

// ===== Constructor / Destructor ===== //
UIEventSystem::UIEventSystem(UIElement* _owner) : UIComponent(_owner)
{
	m_vEvents.resize((unsigned int)UI_EVENTS::UI_EVENT_MAX);
	m_bLockControl = false;
}

UIEventSystem::~UIEventSystem()
{
	using namespace InfernoEngine_UI;


	UnregisterUIEventSystem(this);
}
// ==================================== //

// ===== Interface ===== //
void UIEventSystem::PostInit()
{
	using namespace InfernoEngine_UI;

	RegisterUIEventSystem(this);
}

void UIEventSystem::RegisterEvent(UI_EVENTS _eventType, EventFunction _eventFunction)
{
	if (_eventType >= UI_EVENTS::UI_EVENT_MAX)
		return;

	m_vEvents[(unsigned int)_eventType].push_back(_eventFunction);
}

void UIEventSystem::RegisterForMouseHover(MouseMovementEventFunction _eventFunction)
{
	m_vMouseHoverEvents.push_back(_eventFunction);
}

void UIEventSystem::InvokeEvent(UI_EVENTS _eventType)
{
	if (_eventType >= UI_EVENTS::UI_EVENT_MAX)
		return;

	std::vector<EventFunction> eventFunctions = m_vEvents[(unsigned int)_eventType];

	size_t count = eventFunctions.size();
	for (size_t i = 0; i < count; ++i) {
		eventFunctions[i]();
	}
}

void UIEventSystem::InvokeMouseHover(tagPOINT& _cursorPosition, tagPOINT& _displacement)
{
	size_t count = m_vMouseHoverEvents.size();
	for (size_t i = 0; i < count; ++i) {
		m_vMouseHoverEvents[i](_cursorPosition, _displacement);
	}
}
// ===================== //

// === Mutators === //

// === LockControl
//  IN : bool - Whether or not to lock the control
//  OUT : void
//
//  Sets whether or not that this EventSystem is locked for control, meaning that when it's locked, the mouse cursor doesn't affect any other Event Systems regardless
//  of the position.
void UIEventSystem::LockControl(bool _value)
{
	m_bLockControl = _value;
}
// ================ //

// === Accessors === //

// === IsLockedForControls
//  IN : void
//  OUT : bool - Whether or not this UIEventSystem is Locked for Controls
//
//  Returns whether or not this UIEventSystem is Locked for Controls, meaning that if it is, the mouse will not affect any other EventSystem, regardless of position.
bool UIEventSystem::IsLockedForControls() const
{
	return m_bLockControl;
}
// ================= //