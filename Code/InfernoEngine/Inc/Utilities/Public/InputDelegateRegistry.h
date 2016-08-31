// Author : Jack Gillette
//
// A Delegate Registry Class that handles all the different delegates that could be registered with the InputManager. Handles
// functions for Buttons, Keys, Actions, Axis', Mouse, and MouseMovement
#pragma once

#include "FunctionRegistry.h"
#include "../../Utilities/Public/InputEvents.h"

#include <unordered_map>
#include <vector>
#include <Windows.h>

// === TypeDefs
typedef std::function<void(ACTION_STATUS)> ActionEventFunction;
typedef std::function<void(float)> AxisEventFunction;
typedef std::function<void(POINT, POINT)> MouseMovementEventFunction;
typedef std::function<void(unsigned int)> MouseWheelEventFunction;
typedef std::function<void(bool)> ControlChangeEventFunction;
typedef std::function<void(unsigned int)> ControllerLostEventFunction;
typedef std::function<void(unsigned int)> ControllerGainedEventFunction;

class InputDelegateRegistry
{
private:
	// === Structures
	struct ActionEvent {
		void* CallingObject;
		ActionEventFunction Delegate;
		ActionEvent(void* _callingObject, ActionEventFunction _delegate) {
			CallingObject = _callingObject;
			Delegate = _delegate;
		}
	};

	struct AxisEvent {
		void* CallingObject;
		AxisEventFunction Delegate;
		AxisEvent(void* _callingObject, AxisEventFunction _delegate) {
			CallingObject = _callingObject;
			Delegate = _delegate;
		}
	};

	struct MouseMovementEvent {
		void* CallingObject;
		MouseMovementEventFunction Delegate;
		MouseMovementEvent(void* _callingObject, MouseMovementEventFunction _delegate) {
			CallingObject = _callingObject;
			Delegate = _delegate;
		}
	};

	struct MouseWheelEvent {
		void* CallingObject;
		MouseWheelEventFunction Delegate;
		MouseWheelEvent(void* _callingObject, MouseWheelEventFunction _delegate) {
			CallingObject = _callingObject;
			Delegate = _delegate;
		}
	};

	struct ControlChangeEvent {
		void* CallingObject;
		ControlChangeEventFunction Delegate;
		ControlChangeEvent(void* _callingObject, ControlChangeEventFunction _delegate) {
			CallingObject = _callingObject;
			Delegate = _delegate;
		}
	};

	struct ControllerLostEvent {
		void* CallingObject;
		ControllerLostEventFunction Delegate;
		ControllerLostEvent(void* _callingObject, ControllerLostEventFunction _delegate) {
			CallingObject = _callingObject;
			Delegate = _delegate;
		}
	};

	struct ControllerGainedEvent {
		void* CallingObject;
		ControllerGainedEventFunction Delegate;
		ControllerGainedEvent(void* _callingObject, ControllerGainedEventFunction _delegate) {
			CallingObject = _callingObject;
			Delegate = _delegate;
		}
	};

	struct EventDelegates 
	{
		std::vector<ActionEvent> m_ActionEventDelegates;
		std::vector<AxisEvent> m_AxisEventDelegates;
	};

	// === TypeDefs
	typedef std::unordered_map<unsigned int, EventDelegates>::iterator EventIter;

	// === Variables
	std::unordered_map<unsigned int, EventDelegates> m_RegisteredEventDelegates;
	std::vector<MouseMovementEvent> m_vMouseMovementEventDelegates;
	std::vector<MouseWheelEvent> m_vMouseWheelEventDelegates;
	std::vector<ControlChangeEvent> m_vControlChangeEventDelegates;
	std::vector<ControllerLostEvent> m_vControllerLostEventDelegates;
	std::vector<ControllerGainedEvent> m_vControllerGainedEventDelegates;

public:
	// === Constructor / Destructor === //
	InputDelegateRegistry();
	~InputDelegateRegistry();
	// ================================ //

	// === Interface === //
	
	// === RegisterEventToActionBinding
	//  IN : char* - Action Binding to register with.
	//       ActionEventFunction - Function Pointer that should be registered with the given binding
	//  OUT : unsigned int - Hashed value of the given binding.
	//
	//  Registers the given function to a specified Action Binding, that when the binding triggers an event, it will call this function
	//  and any others registered to it
	unsigned int RegisterEventToActionBinding(const char* _binding, void* _callingObject, ActionEventFunction _function);
	// === RegisterEventToAxisBinding
	//  IN : char* - Axis Binding to register with.
	//       ActionEventFunction - Function Pointer that should be registered with the given binding
	//  OUT : unsigned int - Hashed value of the given binding.
	//
	//  Registers the given function to a specified Axis Binding, that when the binding triggers an event, it will call this function
	//  and any others registered to it
	unsigned int RegisterEventToAxisBinding(const char* _binding, void* _callingObject, AxisEventFunction _function);
	// === RegisterEventToMouseMovement
	//  IN : MouseMovementEventFunction - Function pointer to be registered with MouseMovement Events
	//  OUT : void
	//
	//  Registers the given function to the MouseMovement Event, that when it triggers, it will call this function
	//  and any others registered to it
	void RegisterEventToMouseMovement(void* _callingObject, MouseMovementEventFunction _function);
	// === RegisterEventToMouseWheel
	// IN : MouseWheelEventFunction - Funcion pointer to be registerd with MouseWheel Events
	// OUT : void
	//
	// Registers teh given function to the MouseWheel Event, that when it triggers, it will call this function
	// and any otehrs registered to it
	void RegisterEventToMouseWheel(void* _callingObject, MouseWheelEventFunction _function);
	// === RegisterEventToControlChange
	// IN : ControlChangeEventFunction - Function pointer to be registered with ControlChange Events
	// OUT: void
	//
	// Registers the given function to the ControlChange Event, that when it triggers, it will call this function
	// and any others register to it
	void RegisterEventToControlChange(void* _callingObject, ControlChangeEventFunction _function);
	// === RegisterEventToControllerLost
	// IN : ControllerLostEventFunction - Function pointer to be registered with ControllerLost Events
	// OUT: void
	//
	// Registers the given function to the ControllerLost Event, that when it triggers, it will call this function
	// and any others register to it
	void RegisterEventToControllerLost(void* _callingObject, ControllerLostEventFunction _function);
	// === RegisterEventToControllerGained
	// IN : ControllerGainedEventFunction - Function pointer to be registered with ControllerGained Events
	// OUT: void
	//
	// Registers the given function to the ControllerGained Event, that when it triggers, it will call this function
	// and any others register to it
	void RegisterEventToControllerGained(void* _callingObject, ControllerGainedEventFunction _function);

	// === UnregisterEventFromActionBinding
	//  IN : char* - The name of the Action binding
	//       ActionEventFunction - The delegate to unregister
	//  OUT : bool - Whether or not the delegate was found and removed
	// 
	//  Searches through the registered delegates of a given binding and removes the specified one, if found. Returns true if the
	//  delegate was removed, false otherwise.
	bool UnregisterEventFromActionBinding(const char* _binding, void* _callingObject, ActionEventFunction _function);
	// === UnregisterEventFromActionBinding
	//  IN : unsigned int - The hash key of the Action binding
	//       ActionEventFunction - The delegate to unregister
	//  OUT : bool - Whether or not the delegate was found and removed
	// 
	//  Searches through the registered delegates of a given binding and removes the specified one, if found. Returns true if the
	//  delegate was removed, false otherwise.
	bool UnregisterEventFromActionBinding(unsigned int _hash, void* _callingObject, ActionEventFunction _function);
	// === UnregisterEventFromAxisBinding
	//  IN : char* - The name of the Axis binding
	//       AxisEventFunction - The delegate to unregister
	//  OUT : bool - Whether or not the delegate was found and removed
	// 
	//  Searches through the registered delegates of a given binding and removes the specified one, if found. Returns true if the
	//  delegate was removed, false otherwise.
	bool UnregisterEventFromAxisBinding(const char* _binding, void* _callingObject, AxisEventFunction _function);
	// === UnregisterEventFromAxisBinding
	//  IN : unsigned int - The hash key of the Axis binding
	//       AxisEventFunction - The delegate to unregister
	//  OUT : bool - Whether or not the delegate was found and removed
	// 
	//  Searches through the registered delegates of a given binding and removes the specified one, if found. Returns true if the
	//  delegate was removed, false otherwise.
	bool UnregisterEventFromAxisBinding(unsigned int _hash, void* _callingObject, AxisEventFunction _function);
	// === UnregisterEventFromMouseMovement
	//  IN : MouseMovementEventFunction - The delegate to unregister
	//  OUT : bool - Whether or not the delegate was found and removed
	// 
	//  Searches through the registered delegates for MouseMovement and removes the specified one, if found. Returns true if the
	//  delegate was removed, false otherwise.
	bool UnregisterEventFromMouseMovement(void* _callingObject, MouseMovementEventFunction _function);
	// === UnregisterEventFromMouseWheel
	//  IN : MouseWheelEventFunction - The delegate to unregister
	//  OUT : bool - Whether or not the delegate was found and removed
	// 
	//  Searches through the registered delegates for MouseWheel and removes the specified one, if found. Returns true if the
	//  delegate was removed, false otherwise.
	bool UnregisterEventFromMouseWheel(void* _callingObject, MouseWheelEventFunction _function);
	// === UnregisterEventFromControlChange
	//  IN : ControlChangeEventFunction - The delegate to unregister
	//  OUT: bool - Whether or not the delegate was found and removed
	//
	//  Searches through the registered delegates for ControlChange and removes the specified one, if found. Returns true if the
	//  delegate was removed, false otherwise.
	bool UnregisterEventFromControlChange(void* _callingObject, ControlChangeEventFunction _function);
	// === UnregisterEventFromControllerLost
	//  IN : ControllerLostEventFunction - The delegate to unregister
	//  OUT: bool - Whether or not the delegate was found and removed
	//
	//  Searches through the registered delegates for ControllerLost and removes the specified one, if found. Returns true if the
	//  delegate was removed, false otherwise.
	bool UnregisterEventFromControllerLost(void* _callingObject, ControllerLostEventFunction _function);
	// === UnregisterEventFromControllerGained
	//  IN : ControllerGainedEventFunction - The delegate to unregister
	//  OUT: bool - Whether or not the delegate was found and removed
	//
	//  Searches through the registered delegates for ControllerGained and removes the specified one, if found. Returns true if the
	//  delegate was removed, false otherwise.
	bool UnregisterEventFromControllerGained(void* _callingObject, ControllerGainedEventFunction _function);

	// === InvokeActionEvent
	//  IN : char * - The Action Binding that triggered the event.
	//       ACTION_STATUS - Status of the Action.
	//  OUT : void
	//
	//  Looks up the Registered Delegates of the given binding, then calls all the functions.
	void InvokeActionEvent(const char* _binding, ACTION_STATUS _actionStatus);
	// === InvokeActionEvent
	//  IN : unsigned int - The hash key of the Action Binding that triggered the event.
	//       ACTION_STATUS - Status of the Action.
	//  OUT : void
	//
	//  Looks up the Registered Delegates of the given hash key, then calls all the functions.
	void InvokeActionEvent(unsigned int _hash, ACTION_STATUS _actionStatus);
	// === InvokeAxisEvent
	//  IN : char * - The Axis Binding that triggered the event.
	//       float - Value of the Axis
	//  OUT : void
	//
	//  Looks up the Registered Delegates of the given binding, then calls all the functions.
	void InvokeAxisEvent(const char* _binding, float _value);
	// === InvokeAxisEvent
	//  IN : unsigned int - The hash key of the Axis Binding that triggered the event.
	//       float - Value of the Axis
	//  OUT : void
	//
	//  Looks up the Registered Delegates of the given binding, then calls all the functions.
	void InvokeAxisEvent(unsigned int _hash, float _value);
	// === InvokeMouseMovementEvent
	//  IN : POINT - Current mouse cursor position.
	//       POINT - Displacement of the mouse cursor since it last moved.
	//  OUT : void
	//
	//  Invokes any delegates that are registered to the MouseMovement event.
	void InvokeMouseMovementEvent(POINT _cursorPos, POINT _displacement);
	// === InvokeMouseWheelEvent
	//  IN : unsigned int - Total mouse wheel movement
	//  OUT : void
	//
	//  Invokes any delegates that are registered to the MouseWheel event.
	void InvokeMouseWheelEvent(unsigned int wheelMovement);
	// === InvokeControlChangeEvent
	//  IN : bool
	//  OUT : void
	//
	//  Invokes any delegates that are registered to the ControlChange event.
	void InvokeControlChangeEvent(bool usingController);
	// === InvokeControllerLostEvent
	//  IN : unsigned int - ID of the controller lost
	//  OUT : void
	//
	//  Invokes any delegates that are registered to the ControllerLost event.
	void InvokeControllerLostEvent(unsigned int controllerID);
	// === InvokeControllerGainedEvent
	//  IN : unsigned int - ID of the controller gained
	//  OUT : void
	//
	//  Invokes any delegates that are registered to the ControllerGained event.
	void InvokeControllerGainedEvent(unsigned int controllerID);
	// ================= //
};