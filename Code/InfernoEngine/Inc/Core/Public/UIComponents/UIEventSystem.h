// Author : Jack Gillette
//
// UIComponent that handles registering the UIElement with the UIManager to assure that it will recieve any interaction events.
// When it receives an event, it will call the function that is tied to that specific event.

#pragma once
#include "UIComponent.h"

#include "../../Utilities/Public/UIEvents.h"
#include "InputEvents.h"

#include <vector>

class UIEventSystem : public UIComponent
{
private:
	// === Variables
	std::vector<std::vector<EventFunction>> m_vEvents;
	std::vector<MouseMovementEventFunction> m_vMouseHoverEvents;
	bool m_bLockControl;

public:
	// === Constructor / Destructor === //
	UIEventSystem(UIElement* _owner);
	~UIEventSystem();
	// ================================ //

	// === Interface === //
	virtual void PostInit();

	void RegisterEvent(UI_EVENTS _eventType, EventFunction _eventFunction);
	void RegisterForMouseHover(MouseMovementEventFunction _eventFunction);
	void InvokeEvent(UI_EVENTS _event);
	void InvokeMouseHover(tagPOINT& _cursorPosition, tagPOINT& _displacement);
	// ================= //

	// === Mutators === //

	// === LockControl
	//  IN : bool - Whether or not to lock the control
	//  OUT : void
	//
	//  Sets whether or not that this EventSystem is locked for control, meaning that when it's locked, the mouse cursor doesn't affect any other Event Systems regardless
	//  of the position.
	void LockControl(bool _value);
	// ================ //

	// === Accessors === //

	// === IsLockedForControls
	//  IN : void
	//  OUT : bool - Whether or not this UIEventSystem is Locked for Controls
	//
	//  Returns whether or not this UIEventSystem is Locked for Controls, meaning that if it is, the mouse will not affect any other EventSystem, regardless of position.
	bool IsLockedForControls() const;
	// ================= //
};

