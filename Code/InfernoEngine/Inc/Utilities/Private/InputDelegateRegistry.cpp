#include "InputDelegateRegistry.h"

#include "AssertHelper.h"

// === Constructor / Destructor === //
InputDelegateRegistry::InputDelegateRegistry()
{

}

InputDelegateRegistry::~InputDelegateRegistry()
{

}
// ================================ //

// === Interface === //

// === RegisterEventToActionBinding
//  IN : char* - Action Binding to register with.
//       ActionEventFunction - Function Pointer that should be registered with the given binding
//  OUT : unsigned int - Hashed value of the given binding.
//
//  Registers the given function to a specified Action Binding, that when the binding triggers an event, it will call this function
//  and any others registered to it
unsigned int InputDelegateRegistry::RegisterEventToActionBinding(const char* _binding, void* _callingObject, ActionEventFunction _function)
{
	unsigned int hash = (unsigned int)std::hash<std::string>()(_binding);

	EventIter iter = m_RegisteredEventDelegates.find(hash);

	if (iter == m_RegisteredEventDelegates.end()) {
		EventDelegates delegates;
		delegates.m_ActionEventDelegates.push_back(ActionEvent(_callingObject, _function));
		m_RegisteredEventDelegates.insert(std::make_pair(hash, delegates));
	}
	else {
		iter->second.m_ActionEventDelegates.push_back(ActionEvent(_callingObject, _function));
	}

	return hash;
}

// === RegisterEventToAxisBinding
//  IN : char* - Axis Binding to register with.
//       ActionEventFunction - Function Pointer that should be registered with the given binding
//  OUT : unsigned int - Hashed value of the given binding.
//
//  Registers the given function to a specified Axis Binding, that when the binding triggers an event, it will call this function
//  and any others registered to it
unsigned int InputDelegateRegistry::RegisterEventToAxisBinding(const char* _binding, void* _callingObject, AxisEventFunction _function)
{
	unsigned int hash = (unsigned int)std::hash<std::string>()(_binding);

	EventIter iter = m_RegisteredEventDelegates.find(hash);

	if (iter == m_RegisteredEventDelegates.end()) {
		EventDelegates delegates;
		delegates.m_AxisEventDelegates.push_back(AxisEvent(_callingObject, _function));
		m_RegisteredEventDelegates.insert(std::make_pair(hash, delegates));
	}
	else {
		iter->second.m_AxisEventDelegates.push_back(AxisEvent(_callingObject, _function));
	}

	return hash;
}

// === RegisterEventToMouseMovement
//  IN : MouseMovementEventFunction - Function pointer to be registered with MouseMovement Events
//  OUT : void
//
//  Registers the given function to the MouseMovement Event, that when it triggers, it will call this function
//  and any others registered to it
void InputDelegateRegistry::RegisterEventToMouseMovement(void* _callingObject, MouseMovementEventFunction _function)
{
	m_vMouseMovementEventDelegates.push_back(MouseMovementEvent(_callingObject, _function));
}

// === RegisterEventToMouseWheel
// IN : MouseWheelEventFunction - Funcion pointer to be registerd with MouseWheel Events
// OUT : void
//
// Registers teh given function to the MouseWheel Event, that when it triggers, it will call this function
// and any otehrs registered to it
void InputDelegateRegistry::RegisterEventToMouseWheel(void* _callingObject, MouseWheelEventFunction _function)
{
	m_vMouseWheelEventDelegates.push_back(MouseWheelEvent(_callingObject, _function));
}

// === RegisterEventToControlChange
// IN : ControlChangeEventFunction - Function pointer to be registered with ControlChange Events
// OUT: void
//
// Registers the given function to the ControlChange Event, that when it triggers, it will call this function
// and any others register to it
void InputDelegateRegistry::RegisterEventToControlChange(void* _callingObject, ControlChangeEventFunction _function)
{
	m_vControlChangeEventDelegates.push_back(ControlChangeEvent(_callingObject, _function));
}

// === RegisterEventToControllerLost
// IN : ControllerLostEventFunction - Function pointer to be registered with ControllerLost Events
// OUT: void
//
// Registers the given function to the ControllerLost Event, that when it triggers, it will call this function
// and any others register to it
void InputDelegateRegistry::RegisterEventToControllerLost(void* _callingObject, ControllerLostEventFunction _function)
{
	m_vControllerLostEventDelegates.push_back(ControllerLostEvent(_callingObject, _function));
}

// === RegisterEventToControllerGained
// IN : ControllerGainedEventFunction - Function pointer to be registered with ControllerGained Events
// OUT: void
//
// Registers the given function to the ControllerGained Event, that when it triggers, it will call this function
// and any others register to it
void InputDelegateRegistry::RegisterEventToControllerGained(void* _callingObject, ControllerGainedEventFunction _function)
{
	m_vControllerGainedEventDelegates.push_back(ControllerGainedEvent(_callingObject, _function));
}

// === UnregisterEventFromActionBinding
//  IN : char* - The name of the Action binding
//       ActionEventFunction - The delegate to unregister
//  OUT : bool - Whether or not the delegate was found and removed
// 
//  Searches through the registered delegates of a given binding and removes the specified one, if found. Returns true if the
//  delegate was removed, false otherwise.
bool InputDelegateRegistry::UnregisterEventFromActionBinding(const char* _binding, void* _callingObject, ActionEventFunction _function)
{
	unsigned int hash = (unsigned int)std::hash<std::string>()(_binding);

	EventIter iter = m_RegisteredEventDelegates.find(hash);

	if (iter == m_RegisteredEventDelegates.end()) {
		return false;
	}

	size_t count = iter->second.m_ActionEventDelegates.size();
	for (size_t i = 0; i < count; ++i) {
		if (iter->second.m_ActionEventDelegates[i].CallingObject == _callingObject) {
			iter->second.m_ActionEventDelegates[i] = iter->second.m_ActionEventDelegates[count - 1];
			iter->second.m_ActionEventDelegates.erase(--(iter->second.m_ActionEventDelegates.end()));
			return true;
		}
	}

	return false;
}

// === UnregisterEventFromActionBinding
//  IN : unsigned int - The hash key of the Action binding
//       ActionEventFunction - The delegate to unregister
//  OUT : bool - Whether or not the delegate was found and removed
// 
//  Searches through the registered delegates of a given binding and removes the specified one, if found. Returns true if the
//  delegate was removed, false otherwise.
bool InputDelegateRegistry::UnregisterEventFromActionBinding(unsigned int _hash, void* _callingObject, ActionEventFunction _function)
{
	EventIter iter = m_RegisteredEventDelegates.find(_hash);

	if (iter == m_RegisteredEventDelegates.end()) {
		return false;
	}

	size_t count = iter->second.m_ActionEventDelegates.size();
	for (size_t i = 0; i < count; ++i) {
		if (iter->second.m_ActionEventDelegates[i].CallingObject == _callingObject) {
			iter->second.m_ActionEventDelegates[i] = iter->second.m_ActionEventDelegates[count - 1];
			iter->second.m_ActionEventDelegates.erase(--(iter->second.m_ActionEventDelegates.end()));
			return true;
		}
	}

	return false;
}

// === UnregisterEventFromAxisBinding
//  IN : char* - The name of the Axis binding
//       AxisEventFunction - The delegate to unregister
//  OUT : bool - Whether or not the delegate was found and removed
// 
//  Searches through the registered delegates of a given binding and removes the specified one, if found. Returns true if the
//  delegate was removed, false otherwise.
bool InputDelegateRegistry::UnregisterEventFromAxisBinding(const char* _binding, void* _callingObject, AxisEventFunction _function)
{
	unsigned int hash = (unsigned int)std::hash<std::string>()(_binding);

	EventIter iter = m_RegisteredEventDelegates.find(hash);

	if (iter == m_RegisteredEventDelegates.end()) {
		return false;
	}

	size_t count = iter->second.m_AxisEventDelegates.size();
	for (size_t i = 0; i < count; ++i) {
		if (iter->second.m_AxisEventDelegates[i].CallingObject == _callingObject) {
			iter->second.m_AxisEventDelegates[i] = iter->second.m_AxisEventDelegates[count - 1];
			iter->second.m_AxisEventDelegates.erase(--(iter->second.m_AxisEventDelegates.end()));
			return true;
		}
	}

	return false;
}

// === UnregisterEventFromAxisBinding
//  IN : unsigned int - The hash key of the Axis binding
//       AxisEventFunction - The delegate to unregister
//  OUT : bool - Whether or not the delegate was found and removed
// 
//  Searches through the registered delegates of a given binding and removes the specified one, if found. Returns true if the
//  delegate was removed, false otherwise.
bool InputDelegateRegistry::UnregisterEventFromAxisBinding(unsigned int _hash, void* _callingObject, AxisEventFunction _function)
{
	EventIter iter = m_RegisteredEventDelegates.find(_hash);

	if (iter == m_RegisteredEventDelegates.end()) {
		return false;
	}

	size_t count = iter->second.m_AxisEventDelegates.size();
	for (size_t i = 0; i < count; ++i) {
		if (iter->second.m_AxisEventDelegates[i].CallingObject == _callingObject) {
			iter->second.m_AxisEventDelegates[i] = iter->second.m_AxisEventDelegates[count - 1];
			iter->second.m_AxisEventDelegates.erase(--(iter->second.m_AxisEventDelegates.end()));
			return true;
		}
	}

	return false;
}

// === UnregisterEventFromMouseMovement
//  IN : MouseMovementEventFunction - The delegate to unregister
//  OUT : bool - Whether or not the delegate was found and removed
// 
//  Searches through the registered delegates for MouseMovement and removes the specified one, if found. Returns true if the
//  delegate was removed, false otherwise.
bool InputDelegateRegistry::UnregisterEventFromMouseMovement(void* _callingObject, MouseMovementEventFunction _function)
{
	size_t count = m_vMouseMovementEventDelegates.size();
	for (size_t i = 0; i < count; ++i) {
		if (m_vMouseMovementEventDelegates[i].CallingObject == _callingObject) {
			m_vMouseMovementEventDelegates[i] = m_vMouseMovementEventDelegates[count - 1];
			m_vMouseMovementEventDelegates.erase(--(m_vMouseMovementEventDelegates.end()));
			return true;
		}
	}

	return false;
}

// === UnregisterEventFromMouseWheel
//  IN : MouseWheelEventFunction - The delegate to unregister
//  OUT : bool - Whether or not the delegate was found and removed
// 
//  Searches through the registered delegates for MouseWheel and removes the specified one, if found. Returns true if the
//  delegate was removed, false otherwise.
bool InputDelegateRegistry::UnregisterEventFromMouseWheel(void* _callingObject, MouseWheelEventFunction _function)
{
	size_t count = m_vMouseWheelEventDelegates.size();
	for (size_t i = 0; i < count; ++i) {
		if (m_vMouseWheelEventDelegates[i].CallingObject == _callingObject) {
			m_vMouseWheelEventDelegates[i] = m_vMouseWheelEventDelegates[count - 1];
			m_vMouseWheelEventDelegates.erase(--(m_vMouseWheelEventDelegates.end()));
			return true;
		}
	}

	return false;
}

// === UnregisterEventFromControlChange
//  IN : ControlChangeEventFunction - The delegate to unregister
//  OUT: bool - Whether or not the delegate was found and removed
//
//  Searches through the registered delegates for ControlChange and removes the specified one, if found. Returns true if the
//  delegate was removed, false otherwise.
bool InputDelegateRegistry::UnregisterEventFromControlChange(void* _callingObject, ControlChangeEventFunction _function)
{
	size_t count = m_vControlChangeEventDelegates.size();
	for (size_t i = 0; i < count; ++i) {
		if (m_vControlChangeEventDelegates[i].CallingObject == _callingObject) {
			m_vControlChangeEventDelegates[i] = m_vControlChangeEventDelegates[count - 1];
			m_vControlChangeEventDelegates.erase(--(m_vControlChangeEventDelegates.end()));
			return true;
		}
	}

	return false;
}

// === UnregisterEventFromControllerLost
//  IN : ControllerLostEventFunction - The delegate to unregister
//  OUT: bool - Whether or not the delegate was found and removed
//
//  Searches through the registered delegates for ControllerLost and removes the specified one, if found. Returns true if the
//  delegate was removed, false otherwise.
bool InputDelegateRegistry::UnregisterEventFromControllerLost(void* _callingObject, ControllerLostEventFunction _function)
{
	size_t count = m_vControllerLostEventDelegates.size();
	for (size_t i = 0; i < count; ++i) {
		if (m_vControllerLostEventDelegates[i].CallingObject == _callingObject) {
			m_vControllerLostEventDelegates[i] = m_vControllerLostEventDelegates[count - 1];
			m_vControllerLostEventDelegates.erase(--(m_vControllerLostEventDelegates.end()));
			return true;
		}
	}

	return false;
}

// === UnregisterEventFromControllerGained
//  IN : ControllerGainedEventFunction - The delegate to unregister
//  OUT: bool - Whether or not the delegate was found and removed
//
//  Searches through the registered delegates for ControllerGained and removes the specified one, if found. Returns true if the
//  delegate was removed, false otherwise.
bool InputDelegateRegistry::UnregisterEventFromControllerGained(void* _callingObject, ControllerGainedEventFunction _function)
{
	size_t count = m_vControllerGainedEventDelegates.size();
	for (size_t i = 0; i < count; ++i) {
		if (m_vControllerGainedEventDelegates[i].CallingObject == _callingObject) {
			m_vControllerGainedEventDelegates[i] = m_vControllerGainedEventDelegates[count - 1];
			m_vControllerGainedEventDelegates.erase(--(m_vControllerGainedEventDelegates.end()));
			return true;
		}
	}

	return false;
}

// === InvokeActionEvent
//  IN : char * - The Action Binding that triggered the event.
//       ACTION_STATUS - Status of the Action.
//  OUT : void
//
//  Looks up the Registered Delegates of the given binding, then calls all the functions.
void InputDelegateRegistry::InvokeActionEvent(const char* _binding, ACTION_STATUS _actionStatus)
{
	unsigned int hash = (unsigned int)std::hash<std::string>()(_binding);

	EventIter iter = m_RegisteredEventDelegates.find(hash);

	if (iter == m_RegisteredEventDelegates.end())
		return;

	EventDelegates& delegates = iter->second;
	for (size_t i = 0; i < delegates.m_ActionEventDelegates.size(); ++i) {
		delegates.m_ActionEventDelegates[i].Delegate(_actionStatus);
	}
}

// === InvokeActionEvent
//  IN : unsigned int - The hash key of the Action Binding that triggered the event.
//       ACTION_STATUS - Status of the Action.
//  OUT : void
//
//  Looks up the Registered Delegates of the given hash key, then calls all the functions.
void InputDelegateRegistry::InvokeActionEvent(unsigned int _hash, ACTION_STATUS _actionStatus)
{
	EventIter iter = m_RegisteredEventDelegates.find(_hash);

	if (iter == m_RegisteredEventDelegates.end())
		return;

	EventDelegates& delegates = iter->second;
	size_t count = delegates.m_ActionEventDelegates.size();
	for (size_t i = 0; i < count; ++i) {
		delegates.m_ActionEventDelegates[i].Delegate(_actionStatus);
	}
}

// === InvokeAxisEvent
//  IN : char * - The Axis Binding that triggered the event.
//       float - Value of the Axis
//  OUT : void
//
//  Looks up the Registered Delegates of the given binding, then calls all the functions.
void InputDelegateRegistry::InvokeAxisEvent(const char* _binding, float _value)
{
	unsigned int hash = (unsigned int)std::hash<std::string>()(_binding);

	EventIter iter = m_RegisteredEventDelegates.find(hash);

	if (iter == m_RegisteredEventDelegates.end())
		return;

	EventDelegates& delegates = iter->second;
	size_t count = delegates.m_AxisEventDelegates.size();
	for (size_t i = 0; i < count; ++i) {
		delegates.m_AxisEventDelegates[i].Delegate(_value);
	}
}

// === InvokeAxisEvent
//  IN : unsigned int - The hash key of the Axis Binding that triggered the event.
//       float - Value of the Axis
//  OUT : void
//
//  Looks up the Registered Delegates of the given binding, then calls all the functions.
void InputDelegateRegistry::InvokeAxisEvent(unsigned int _hash, float _value)
{
	EventIter iter = m_RegisteredEventDelegates.find(_hash);

	if (iter == m_RegisteredEventDelegates.end())
		return;

	EventDelegates& delegates = iter->second;
	size_t count = delegates.m_AxisEventDelegates.size();
	for (size_t i = 0; i < count; ++i) {
		delegates.m_AxisEventDelegates[i].Delegate(_value);
	}
}

// === InvokeMouseMovementEvent
//  IN : POINT - Current mouse cursor position.
//       POINT - Displacement of the mouse cursor since it last moved.
//  OUT : void
//
//  Invokes any delegates that are registered to the MouseMovement event.
void InputDelegateRegistry::InvokeMouseMovementEvent(POINT _cursorPos, POINT _displacement)
{
	size_t count = m_vMouseMovementEventDelegates.size();
	for (size_t i = 0; i < count; ++i) {
		m_vMouseMovementEventDelegates[i].Delegate(_cursorPos, _displacement);
	}
}

// === InvokeMouseWheelEvent
//  IN : unsigned int - Total mouse wheel movement
//  OUT : void
//
//  Invokes any delegates that are registered to the MouseWheel event.
void InputDelegateRegistry::InvokeMouseWheelEvent(unsigned int wheelMovement)
{
	size_t count = m_vMouseWheelEventDelegates.size();
	for (size_t i = 0; i < count; ++i) {
		m_vMouseWheelEventDelegates[i].Delegate(wheelMovement);
	}
}

// === InvokeControlChangeEvent
//  IN : bool
//  OUT : void
//
//  Invokes any delegates that are registered to the ControlChange event.
void InputDelegateRegistry::InvokeControlChangeEvent(bool usingController)
{
	size_t count = m_vControlChangeEventDelegates.size();
	for (size_t i = 0; i < count; ++i) {
		m_vControlChangeEventDelegates[i].Delegate(usingController);
	}
}

// === InvokeControllerLostEvent
//  IN : unsigned int - ID of the controller lost
//  OUT : void
//
//  Invokes any delegates that are registered to the ControllerLost event.
void InputDelegateRegistry::InvokeControllerLostEvent(unsigned int controllerID)
{
	size_t count = m_vControllerLostEventDelegates.size();
	for (size_t i = 0; i < count; ++i) {
		m_vControllerLostEventDelegates[i].Delegate(controllerID);
	}
}

// === InvokeControllerGainedEvent
//  IN : unsigned int - ID of the controller gained
//  OUT : void
//
//  Invokes any delegates that are registered to the ControllerGained event.
void InputDelegateRegistry::InvokeControllerGainedEvent(unsigned int controllerID)
{
	size_t count = m_vControllerGainedEventDelegates.size();
	for (size_t i = 0; i < count; ++i) {
		m_vControllerGainedEventDelegates[i].Delegate(controllerID);
	}
}
// ================= //