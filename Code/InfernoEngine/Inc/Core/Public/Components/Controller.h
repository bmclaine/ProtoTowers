// Author: Jonathan Gyurkovics
//
// An interface class which can be derived off of to allow for registering for input events from the input manager
#pragma once

#include "Components\IComponent.h"
#include "Keys.h"
#include "Buttons.h"
#include "InputEvents.h"

class Controller : public IComponent
{
public:
	// IN: void
	// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
	//
	// Initializes object variables
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap);

protected:
	Controller(GameObject* const gameObject, Transform* const transform);
	virtual ~Controller() = 0;

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
	bool RegisterBinding(std::string bindingName, Button positiveMouseButton = Button::None, Button negativeMouseButton = Button::None, Key positiveKey = Key::None, Key negativeKey = Key::None,
		Button positiveButton = Button::None, Button negativeButton = Button::None, Axis axis = Axis::None, unsigned int controller = 0, bool inverse = false);
	// IN: string              - binding to register to
	//     ActionEventFunction - pointer to the function to receive the event
	// OUT: void
	//
	// Registers the function for the event from the binding requested
	void RegisterForKeyEvent(std::string bindingName, void* _callingObject, ActionEventFunction function);
	// IN: string            - binding to register to
	//     AxisEventFunction - pointer to the function to receive the event
	// OUT: void
	//
	// Registers the function to the axis event requested
	void RegisterForAxisEvent(std::string bindingName, void* _callingObject, AxisEventFunction function);
	// IN: MouseMovementEventFunction - pointer to the function to receive the event
	// OUT: void
	//
	// Registers the function to the mouse movement event
	void RegisterForMouseMovementEvent(void* _callingObject, MouseMovementEventFunction function);
	// IN: MouseWheelEventFunction - pointer to the function to receive the event
	// OUT: void
	//
	// Registers the function to the mouse wheel event
	void RegisterForMouseWheelEvent(void* _callingObject, MouseWheelEventFunction function);

	// IN: string - binding to unregister from
	// OUT: bool - status of the unbinding, false if not found
	//
	// Unbinds the requested binding
	bool UnregisterBinding(std::string bindingName);
	// IN: string              - name of the binding
	//     ActionEventFunction - function to unbind
	// OUT: void
	//
	// Unbinds the function from the specified binding
	void UnregisterForKeyEvent(std::string bindingName, void* _callingObject, ActionEventFunction function);
	// IN: string            - name of the binding
	//     AxisEventFunction - function to unbind
	// OUT: void
	//
	// Unbinds the functoin from the specified binding
	void UnregisterForAxisEvent(std::string bindingName, void* _callingObject, AxisEventFunction function);
	// IN: MouseMovementEventFunction - function to unbind
	// OUT: void
	//
	// Unbinds the functoin from the specified binding
	void UnregisterForMouseMovementEvent(void* _callingObject, MouseMovementEventFunction function);
	// IN: MouseWheelEventFunction - function to unbind
	// OUT: void
	//
	// Unbinds the functoin from the specified binding
	void UnregisterForMouseWheelEvent(void* _callingObject, MouseWheelEventFunction function);
};

