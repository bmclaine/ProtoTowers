// Author: Jonathan Gyurkovics
//
// Handles Keyboard, Mouse, and Controller inputs

#pragma once

#include "../../Utilities/Public/Keys.h"
#include "../../Utilities/Public/Buttons.h"
#include "../../Utilities/Public/Flags.h"
#include "../../Utilities/Public/InfernoTimer.h"
#include "../../Utilities/Public/InputEvents.h"

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include <set>
#include <unordered_map>
#include <Xinput.h>
#pragma comment(lib, "xinput.lib")

class InputDelegateRegistry;

class InputManager
{
private:
	enum KEY_STATES { KEY_PRESSED = 128, KEY_DOWN = 128, KEY_RELEASED = 64, KEY_MASK = KEY_PRESSED | KEY_RELEASED };

	struct InputCheck
	{
		Button	PositiveMouseButton;
		Button	NegativeMouseButton;
		Key		PositiveKey;
		Key		NegativeKey;
		Button	PositiveButton;
		Button	NegativeButton;
		Axis	Axis;
		unsigned int Controller;
		bool	Inverse;
	};

	static InputDelegateRegistry s_pDelegateRegistry;
	static std::unordered_map<std::string, InputCheck> s_umKeyBindings;
	static XINPUT_STATE s_ControllerInputStates[XUSER_MAX_COUNT];
	static Flags<char> s_ConnectedControllers;
	static std::set<unsigned int> s_setRegisteredKeys;
	static POINT s_WindowPosition;
	static POINT s_prevMousePosition;
	static POINT s_mouseMovement;
	static HWND s_hWindow;
	static int s_iMouseWheel;
	static int s_iMouseWheelChange;
	static float s_axisValues[XUSER_MAX_COUNT][TOTAL_AXIS];
	static SHORT s_ThumbDeadZone;
	static BYTE s_TriggerThreshold;
	static char s_keys[256];
	static char s_changes[256];
	static char s_cMouseButtons;
	static char s_cMouseButtonStates[TOTAL_MOUSE_BUTTONS];
	static char s_buttonStates[XUSER_MAX_COUNT][TOTAL_BUTTONS];
	static bool s_bLostFocus;
	static bool s_bShowMouse;
	static bool s_bUsingController;
	static bool s_bPrevUsingController;

public:
	// State is the mouse should be reset to the center of the screen each frame
	static bool s_CenterMouse;
	static bool s_CheckAllKeys;

private:

	// IN: int - key that was pressed
	// OUT: void
	//
	// Sets the key passed in as down
	static void KeyDown(int key);
	// IN: int - key that was released
	// OUT: void
	//
	// Sets the key passed in as up
	static void KeyUp(int key);
	// IN: int - mouse button that was pressed
	// OUT: void
	//
	// Sets the mouse button passed in as down
	static void MouseButtonDown(int mbutton);
	// IN: int - mouse button that was released
	// OUT: void
	//
	// Sets the mouse button passed in as up
	static void MouseButtonUp(int mbutton);
	// IN: int - mouse wheel direction
	// OUT: void
	//
	// Sets the mouse wheel direction for this frame
	static void MouseWheelDirection(int mouseWheel);
	// IN: int - window x position
	// IN: int - window y position
	// OUT: void
	//
	// Updates the windows top left position
	static void UpdateWindowPosition(int x, int y);
	// IN: bool - Lost Window Focus
	// OUT: void
	//
	// Tells the InputManager if we lost window focus
	static void LostWindowFocus(bool focus);

public:
	// IN: HWND - handle to the window that will be used by the input manager.
	//            Only needed for centering purposes so nullptr can be passed and it will use the desktop
	// OUT: bool - States if the manager set up correctly
	//
	// Initializes the input manager
	static bool Initialize(HWND window);

	// IN: void
	// OUT: bool - Status of if the controller is being used
	//
	// Returns the status of the use of the controller
	static bool UsingController();

	// IN: Key - Key to register
	// OUT: void
	//
	// Registers the key if it has not already been registered
	static void RegisterKey(Key key);

	// IN: char*  - Name for the binding
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
	static bool RegisterBinding(std::string bindingName, Button positiveMouseButton = Button::None, Button negativeMouseButton = Button::None, Key positiveKey = Key::None, Key negativeKey = Key::None,
		Button positiveButton = Button::None, Button negativeButton = Button::None, Axis axis = Axis::None, unsigned int controller = 0, bool inverse = false);

	// IN: char*  - Name of the binding to update
	//     Button - Mouse button to update to
	// OUT: bool - status of the update
	//
	// Updates the mouse button to the new button
	static bool UpdatePositiveMouseButton(std::string bindingName, Button positiveMouseButton);
	// IN: char*  - Name of the binding to update
	//     Button - Mouse button to update to
	// OUT: bool - status of the update
	//
	// Updates the mouse button to the new button
	static bool UpdateNegativeMouseButton(std::string bindingName, Button negativeMouseButton);
	// IN: char* - Name of the binding to update
	//     Key   - Key to update to
	// OUT: bool - status of the update
	//
	// Updates the key to the new button
	static bool UpdatePositiveKey(std::string bindingName, Key PositiveKey);
	// IN: char* - Name of the binding to update
	//     Key   - Key to update to
	// OUT: bool - status of the update
	//
	// Updates the key to the new button
	static bool UpdateNegativeKey(std::string bindingName, Key NegativeKey);
	// IN: char*  - Name of the binding to update
	//     Button - Button to update to
	// OUT: bool - status of the update
	//
	// Updates the button to the new button
	static bool UpdatePositiveButton(std::string bindingName, Button positiveButton);
	// IN: char*  - Name of the binding to update
	//     Button - Mouse button to update to
	// OUT: bool - status of the update
	//
	// Updates the mouse button to the new button
	static bool UpdateNegativeButton(std::string bindingName, Button negativeButton);
	// IN: char* - Name of the binding to update
	//     Axis  - Axis to update to
	// OUT: bool - status of the update
	//
	// Updates the axis to the new button
	static bool UpdateBoundAxis(std::string bindingName, Axis axis);
	// IN: char* - Name of the binding to update
	//     bool  - update the inverse to
	// OUT: bool - status of the update
	//
	// Updates the inverse bool to the new value
	static bool UpdateBoundInverse(std::string bindingName, bool inverse);

	// IN: char* - Name of the binding to check
	// OUT: bool - status of key pressed
	//
	// Checks all bound Keys and Buttons and returns true if any are pressed this frame
	static bool BindingButtonPressed(std::string bindingName);
	// IN: char* - Name of the binding to check
	// OUT: bool - status of key pressed
	//
	// Checks all bound inputs and returns true if any are down this frame
	static bool BindingButtonDown(std::string bindingName);
	// IN: char* - Name of the binding to check
	// OUT: bool - status of key pressed
	//
	// Checks all bound Keys and Buttons and returns true if any are released this frame
	static bool BindingButtonReleased(std::string bindingName);

	// IN: char* - Name of the binding to check
	// OUT: float - value of the axis
	//
	// Returns between a -1.0f to 1.0f based on the bound inputs. Buttons and Keys will return flat
	// values where axis will return between values.
	static float BindingAxisRaw(std::string bindingName);

	// IN: char* - Name of binding to remove
	// OUT: bool - Status of unbinding
	//
	// Removes the binding if it exists
	static bool UnregisterBinding(std::string bindingName);

	// IN: Key - Key to unregister
	// OUT: void
	//
	// Unregisters the key if it is registered
	static void UnregisterKey(Key key);
	
	// IN: void
	// OUT: bool - States if the manager updated successfully
	//
	// Updates the input manager from all changes from last frame
	static bool Update();

	// IN: Button       - Button to check
	// OUT: bool - Status of the button
	//
	// Checks the button to see if it was pressed this frame
	static bool IsMouseButtonPressed(Button button);
	// IN: Button       - Button to check
	// OUT: bool - Status of the button
	//
	// Checks the button to see if it is down
	static bool IsMouseButtonDown(Button button);
	// IN: Button       - Button to check
	// OUT: bool - Status of the button
	//
	// Checks the button to see if it was released this frame
	static bool IsMouseButtonReleased(Button button);

	// IN: void
	// OUT: bool - States if any button was pressed
	//
	// Checks to see if any button has been pressed this frame
	static bool IsAnyMouseButtonPressed();
	// IN: void
	// OUT: bool - States if any button is down
	//
	// Checks to see if any button is currently down
	static bool IsAnyMouseButtonDown();
	// IN: void
	// OUT: bool - States if any button was released
	//
	// Checks to see if any button has been released this frame
	static bool IsAnyMouseButtonReleased();

	// IN: Key - Key to check
	// OUT: bool - Status of the key
	//
	// Checks the key to see if it was pressed this frame
	static bool IsKeyPressed(Key key);
	// IN: Key - Key to check
	// OUT: bool - Status of the key
	//
	// Checks the key to see if it is down
	static bool IsKeyDown(Key key);
	// IN: Key - Key to check
	// OUT: bool - Status of the key
	//
	// Checks the key to see if it was released this frame
	static bool IsKeyReleased(Key key);

	// IN: void
	// OUT: bool - States if any key was pressed
	//
	// Checks to see if any key has been pressed this frame
	static bool IsAnyKeyPressed();
	// IN: void
	// OUT: bool - States if any key is down
	//
	// Checks to see if any key is currently down
	static bool IsAnyKeyDown();
	// IN: void
	// OUT: bool - States if any key was released
	//
	// Checks to see if any key has been released this frame
	static bool IsAnyKeyReleased();

	// IN: unsigned int - Controller to check 
	//     Button       - Button to check
	// OUT: bool - Status of the button
	//
	// Checks the button to see if it was pressed this frame
	static bool IsButtonPressed(unsigned int controller, Button button);
	// IN: unsigned int - Controller to check 
	//     Button       - Button to check
	// OUT: bool - Status of the button
	//
	// Checks the button to see if it is down
	static bool IsButtonDown(unsigned int controller, Button button);
	// IN: unsigned int - Controller to check 
	//     Button       - Button to check
	// OUT: bool - Status of the button
	//
	// Checks the button to see if it was released this frame
	static bool IsButtonReleased(unsigned int controller, Button button);

	// IN: unsigned int - Controller to check 
	// OUT: bool - States if any button was pressed
	//
	// Checks to see if any button has been pressed this frame
	static bool IsAnyButtonPressed(unsigned int controller);
	// IN: unsigned int - Controller to check 
	// OUT: bool - States if any button is down
	//
	// Checks to see if any button is currently down
	static bool IsAnyButtonDown(unsigned int controller);
	// IN: unsigned int - Controller to check 
	// OUT: bool - States if any button was released
	//
	// Checks to see if any button has been released this frame
	static bool IsAnyButtonReleased(unsigned int controller);

	// IN: unsigned int - Controller to check
	//     Axis         - Axis to check
	// OUT: int - Range of the axis
	//
	// Checks the axis of the controller specified and returns the value
	static float GetAxis(unsigned int controller, Axis axis);

	// IN: void
	// OUT: int - MouseWheel Direction
	//
	// Returns the direction of the mouse wheel this frame
	// 1 - Wheel Up, 0 - No Movement, -1 - Wheel Down
	static int GetMouseWheel();

	// IN: HWND - Window to use by the manager
	// OUT: void
	//
	// Sets the incoming window as the window to use for centering the mouse
	static void SetWindow(HWND window);
	// IN: SHORT - DeadZone to use
	// OUT: void
	//
	// Sets the deadzone to the specified amount if in range
	static void SetThumbDeadZone(SHORT deadzone);
	// IN: BYTE - Threshold to use
	// OUT: void
	//
	// Sets the threshold to the specified amount if in range
	static void SetTriggerThreshold(BYTE threshold);
	// IN: bool - States the visibilty of the mouse
	// OUT: void
	//
	// Sets the mouse's visibility based on the incoming bool
	static void ShowMouse(bool visible);
	// IN: void
	// OUT: POINT - Movement made by the mouse
	//
	// Returns the total movement the mouse has made since last frame
	static POINT GetMouseMovement();
	// IN: void
	// OUT: POINT - Current mouse position on screen
	//
	// Returns the current mouse position on screen
	static POINT GetMousePosition();

	// IN : char* - The name of the binding
	//      ActionEventFunction - The delegate function to register to the binding
	// OUT : void
	//
	// Registers a delegate function to a specific Action, that will be called when there's an update for the Action.
	static void RegisterEventToActionBinding(const char* _binding, void* _callingObject, ActionEventFunction _delegate);

	// IN : char* - The name of the binding
	//      AxisEventFunction - The delegate function to register to the binding
	// OUT : void
	//
	// Registers a delegate function to a specific Axis, that will be called when there's an update for that Axis.
	static void RegisterEventToAxisBinding(const char* _binding, void* _callingObject, AxisEventFunction _delegate);

	// IN : MouseMovementEventFunction - The delegate to register to MouseMovement Event.
	// OUT : void
	// 
	// Registers a delegate to the MouseMovement Event, that will be called anytime the mouse moves.
	static void RegisterEventToMouseMovement(void* _callingObject, MouseMovementEventFunction _delegate);

	// IN : MouseWheelEventFunction - The delegate to register to MouseWheel Event.
	// OUT : void
	// 
	// Registers a delegate to the MouseWheel Event, that will be called anytime the mouse moves.
	static void RegisterEventToMouseWheel(void* _callingObject, MouseWheelEventFunction _delegate);

	// IN : ControlChangeEventFunction - The delegate to register to ControlChange Event
	// OUT : void
	//
	// Registers a delegate to the ControlChange Event, that will be called anytime the control changes between mouse and controller
	static void RegisterEventToControlChange(void* _callingObject, ControlChangeEventFunction _delegate);

	// IN : ControllerLostEventFunction - The delegate to register to ControllerLost Event
	// OUT : void
	//
	// Registers a delegate to the ControllerLost Event, that will be called if a controller is unplugged
	static void RegisterEventToControllerLost(void* _callingObject, ControllerLostEventFunction _delegate);

	// IN : ControllerGainedEventFunction - The delegate to register to ControllerGained Event
	// OUT : void
	//
	// Registers a delegate to the ControllerGained Event, that will be called if a controller is plugged in
	static void RegisterEventToControllerGained(void* _callingObject, ControllerGainedEventFunction _delegate);

	// IN : char* - The name of the binding
	//      ActionEventFunction - The delegate function to unregister
	// OUT : void
	//
	// Unregisters a delegate function from a specific Action.
	static bool UnregisterEventFromAction(const char* _binding, void* _callingObject, ActionEventFunction _delegate);

	// IN : char* - The name of the binding
	//      ActionEventFunction - The delegate function to unregister
	// OUT : void
	//
	// Unregisters a delegate function from a specific Action.
	static bool UnregisterEventFromAction(unsigned int _hash, void* _callingObject, ActionEventFunction _delegate);

	// IN : char* - The name of the binding
	//      AxisEventFunction - The delegate function to unregister
	// OUT : void
	//
	// Unregisters a delegate function from a specific Axis.
	static bool UnregisterEventFromAxis(const char* _binding, void* _callingObject, AxisEventFunction _delegate);

	// IN : char* - The name of the binding
	//      AxisEventFunction - The delegate function to unregister
	// OUT : void
	//
	// Unregisters a delegate function from a specific Axis.
	static bool UnregisterEventFromAxis(unsigned int _hash, void* _callingObject, AxisEventFunction _delegate);

	// IN : MouseMovementEventFunction - The delegate function to unregister
	// OUT : void
	//
	// Unregisters a delegate function from a specific Action.
	static bool UnregisterEventFromMouseMovement(void* _callingObject, MouseMovementEventFunction _delegate);

	// IN : MouseWheelEventFunction - The delegate function to unregister
	// OUT : void
	//
	// Unregisters a delegate function from a specific Action.
	static bool UnregisterEventFromMouseWheel(void* _callingObject, MouseWheelEventFunction _delegate);

	// IN : ControlChangeEventFunction - The delegate function to unregister
	// OUT : void
	//
	// Unregisters a delegate function from a specific Action.
	static bool UnregisterEventFromControlChange(void* _callingObject, ControlChangeEventFunction _delegate);

	// IN : ControllerLostEventFunction - The delegate function to unregister
	// OUT : void
	//
	// Unregisters a delegate function from a specific Action.
	static bool UnregisterEventFromControllerLost(void* _callingObject, ControllerLostEventFunction _delegate);

	// IN : ControllerGainedEventFunction - The delegate function to unregister
	// OUT : void
	//
	// Unregisters a delegate function from a specific Action.
	static bool UnregisterEventFromControllerGained(void* _callingObject, ControllerGainedEventFunction _delegate);

	friend class InfernoEngine;
};

