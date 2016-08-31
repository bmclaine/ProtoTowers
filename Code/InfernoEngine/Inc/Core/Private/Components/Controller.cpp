#include "Components\Controller.h"
#include "InputManager.h"

Controller::Controller(GameObject* const gameObject, Transform* const transform) : IComponent(gameObject, transform) {}

Controller::~Controller() {}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes object variables
void Controller::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap){

}

// IN: string - Name for the binding
//     Button - Positive Mouse Button
//     Button - Negative Mouse Button
//     Key    - Positive Key
//     Key    - Negative Key
//     Button - Positive Controller Key
//     Button - Negative Controller Key
//     Axis   - Axis to track
//     bool   - Inverse bool for float results
// OUT: bool - Fails if already registered
//
// Registers the incoming data to the string for input
bool Controller::RegisterBinding(std::string bindingName, Button positiveMouseButton, Button negativeMouseButton, Key positiveKey, Key negativeKey,
	Button positiveButton, Button negativeButton, Axis axis, unsigned int controller, bool inverse)
{
	return InputManager::RegisterBinding(bindingName, positiveMouseButton, negativeMouseButton, positiveKey, negativeKey, positiveButton, negativeButton, axis, controller, inverse);
}
// IN: string              - binding to register to
//     ActionEventFunction - pointer to the function to receive the event
// OUT: void
//
// Registers the function for the event from the binding requested
void Controller::RegisterForKeyEvent(std::string bindingName, void* _callingObject, ActionEventFunction function)
{
	InputManager::RegisterEventToActionBinding(bindingName.c_str(), _callingObject, function);
}
// IN: string            - binding to register to
//     AxisEventFunction - pointer to the function to receive the event
// OUT: void
//
// Registers the function to the axis event requested
void Controller::RegisterForAxisEvent(std::string bindingName, void* _callingObject, AxisEventFunction function)
{
	InputManager::RegisterEventToAxisBinding(bindingName.c_str(), _callingObject, function);
}
// IN: MouseMovementEventFunction - pointer to the function to receive the event
// OUT: void
//
// Registers the function to the mouse movement event
void Controller::RegisterForMouseMovementEvent(void* _callingObject, MouseMovementEventFunction function)
{
	InputManager::RegisterEventToMouseMovement(_callingObject, function);
}
// IN: MouseWheelEventFunction - pointer to the function to receive the event
// OUT: void
//
// Registers the function to the mouse wheel event
void Controller::RegisterForMouseWheelEvent(void* _callingObject, MouseWheelEventFunction function)
{
	InputManager::RegisterEventToMouseWheel(_callingObject, function);
}

// IN: string - binding to unregister from
// OUT: bool - status of the unbinding, false if not found
//
// Unbinds the requested binding
bool Controller::UnregisterBinding(std::string bindingName)
{
	return InputManager::UnregisterBinding(bindingName);
}
// IN: string              - name of the binding
//     ActionEventFunction - function to unbind
// OUT: void
//
// Unbinds the function from the specified binding
void Controller::UnregisterForKeyEvent(std::string bindingName, void* _callingObject, ActionEventFunction function)
{
	InputManager::UnregisterEventFromAction(bindingName.c_str(), _callingObject, function);
}
// IN: string            - name of the binding
//     AxisEventFunction - function to unbind
// OUT: void
//
// Unbinds the functoin from the specified binding
void Controller::UnregisterForAxisEvent(std::string bindingName, void* _callingObject, AxisEventFunction function)
{
	InputManager::UnregisterEventFromAxis(bindingName.c_str(), _callingObject, function);
}
// IN: MouseMovementEventFunction - function to unbind
// OUT: void
//
// Unbinds the functoin from the specified binding
void Controller::UnregisterForMouseMovementEvent(void* _callingObject, MouseMovementEventFunction function)
{
	InputManager::UnregisterEventFromMouseMovement(_callingObject, function);
}
// IN: MouseWheelEventFunction - function to unbind
// OUT: void
//
// Unbinds the functoin from the specified binding
void Controller::UnregisterForMouseWheelEvent(void* _callingObject, MouseWheelEventFunction function)
{
	InputManager::UnregisterEventFromMouseWheel(_callingObject, function);
}