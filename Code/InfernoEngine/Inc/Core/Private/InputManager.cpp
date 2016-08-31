#include "InputManager.h"

#include "AssertHelper.h"
#include "InputDelegateRegistry.h"

#define CONTROLLER_CONNECTION_CHECK 500u
#define MAX_DEADZONE 32767
#define MAX_DEADZONE_NEG 32768
#define MAX_THRESHOLD 255

InputDelegateRegistry InputManager::s_pDelegateRegistry;
std::unordered_map<std::string, InputManager::InputCheck> InputManager::s_umKeyBindings;
XINPUT_STATE InputManager::s_ControllerInputStates[XUSER_MAX_COUNT];
Flags<char> InputManager::s_ConnectedControllers(4);
std::set<unsigned int> InputManager::s_setRegisteredKeys;
POINT InputManager::s_WindowPosition;
POINT InputManager::s_prevMousePosition;
POINT InputManager::s_mouseMovement;
HWND InputManager::s_hWindow;
int InputManager::s_iMouseWheel;
int InputManager::s_iMouseWheelChange;
float InputManager::s_axisValues[XUSER_MAX_COUNT][TOTAL_AXIS];
SHORT InputManager::s_ThumbDeadZone = 8689;
BYTE InputManager::s_TriggerThreshold = 30;
char InputManager::s_keys[256];
char InputManager::s_changes[256];
bool InputManager::s_CenterMouse;
bool InputManager::s_CheckAllKeys;
char InputManager::s_cMouseButtons;
char InputManager::s_cMouseButtonStates[TOTAL_MOUSE_BUTTONS];
char InputManager::s_buttonStates[XUSER_MAX_COUNT][TOTAL_BUTTONS];
bool InputManager::s_bLostFocus = false;
bool InputManager::s_bShowMouse = true;
bool InputManager::s_bUsingController = false;
bool InputManager::s_bPrevUsingController = false;

// IN: int - key that was pressed
// OUT: void
//
// Sets the key passed in as down
void InputManager::KeyDown(int key) { s_changes[key] |= KEY_PRESSED; }
// IN: int - key that was released
// OUT: void
//
// Sets the key passed in as up
void InputManager::KeyUp(int key) { s_changes[key] = 0; }
// IN: int - mouse button that was pressed
// OUT: void
//
// Sets the mouse button passed in as down
void InputManager::MouseButtonDown(int mbutton) { s_cMouseButtons |= (1 << mbutton); }
// IN: int - mouse button that was released
// OUT: void
//
// Sets the mouse button passed in as up
void InputManager::MouseButtonUp(int mbutton) { s_cMouseButtons &= ~(1 << mbutton); }
// IN: int - mouse wheel direction
// OUT: void
//
// Sets the mouse wheel direction for this frame
void InputManager::MouseWheelDirection(int mouseWheel) { s_iMouseWheelChange = mouseWheel; }
// IN: int - window x position
// IN: int - window y position
// OUT: void
//
// Updates the windows top left position
void InputManager::UpdateWindowPosition(int x, int y)
{
	s_WindowPosition.x = x;
	s_WindowPosition.y = y;
}
// IN: bool - Lost Window Focus
// OUT: void
//
// Tells the InputManager if we lost window focus
void InputManager::LostWindowFocus(bool focus)
{
	s_bLostFocus = focus;

	if (focus)
		ShowMouse(true);
	else
		ShowMouse(s_bShowMouse);
}

// IN: Key - Key to register
// OUT: void
//
// Registers the key if it has not already been registered
void InputManager::RegisterKey(Key key)
{
	auto iter = s_setRegisteredKeys.find((int)key);

	if (iter == s_setRegisteredKeys.end())
		s_setRegisteredKeys.insert((int)key);
}

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
bool InputManager::RegisterBinding(std::string bindingName, Button positiveMouseButton, Button negativeMouseButton, Key positiveKey, Key negativeKey,
	Button positiveButton, Button negativeButton, Axis axis, unsigned int controller, bool inverse)
{
	if (s_umKeyBindings.find(bindingName) != s_umKeyBindings.end())
		return false;

	if (positiveKey != Key::None)
		RegisterKey(positiveKey);
	if (negativeKey != Key::None)
		RegisterKey(negativeKey);

	InputCheck newCheck = {
		positiveMouseButton,
		negativeMouseButton,
		positiveKey,
		negativeKey,
		positiveButton,
		negativeButton,
		axis,
		controller,
		inverse
	};

	s_umKeyBindings.insert(std::pair<std::string, InputCheck>(bindingName, newCheck));
	return true;
}

// IN: char*  - Name of the binding to update
//     Button - Mouse button to update to
// OUT: bool - status of the update
//
// Updates the mouse button to the new button
bool InputManager::UpdatePositiveMouseButton(std::string bindingName, Button positiveMouseButton)
{
	auto iter = s_umKeyBindings.find(bindingName);
	if (iter == s_umKeyBindings.end())
		return false;

	iter->second.PositiveMouseButton = positiveMouseButton;

	return true;
}
// IN: char*  - Name of the binding to update
//     Button - Mouse button to update to
// OUT: bool - status of the update
//
// Updates the mouse button to the new button
bool InputManager::UpdateNegativeMouseButton(std::string bindingName, Button negativeMouseButton)
{
	auto iter = s_umKeyBindings.find(bindingName);
	if (iter == s_umKeyBindings.end())
		return false;

	iter->second.NegativeMouseButton = negativeMouseButton;

	return true;
}
// IN: char* - Name of the binding to update
//     Key   - Key to update to
// OUT: bool - status of the update
//
// Updates the key to the new button
bool InputManager::UpdatePositiveKey(std::string bindingName, Key PositiveKey)
{
	auto iter = s_umKeyBindings.find(bindingName);
	if (iter == s_umKeyBindings.end())
		return false;

	UnregisterKey(iter->second.PositiveKey);
	RegisterKey(PositiveKey);

	iter->second.PositiveKey = PositiveKey;

	return true;
}
// IN: char* - Name of the binding to update
//     Key   - Key to update to
// OUT: bool - status of the update
//
// Updates the key to the new button
bool InputManager::UpdateNegativeKey(std::string bindingName, Key NegativeKey)
{
	auto iter = s_umKeyBindings.find(bindingName);
	if (iter == s_umKeyBindings.end())
		return false;

	UnregisterKey(iter->second.NegativeKey);
	RegisterKey(NegativeKey);

	iter->second.NegativeKey = NegativeKey;

	return true;
}
// IN: char*  - Name of the binding to update
//     Button - Button to update to
// OUT: bool - status of the update
//
// Updates the button to the new button
bool InputManager::UpdatePositiveButton(std::string bindingName, Button positiveButton)
{
	auto iter = s_umKeyBindings.find(bindingName);
	if (iter == s_umKeyBindings.end())
		return false;

	iter->second.PositiveButton = positiveButton;

	return true;
}
// IN: char*  - Name of the binding to update
//     Button - Mouse button to update to
// OUT: bool - status of the update
//
// Updates the mouse button to the new button
bool InputManager::UpdateNegativeButton(std::string bindingName, Button negativeButton)
{
	auto iter = s_umKeyBindings.find(bindingName);
	if (iter == s_umKeyBindings.end())
		return false;

	iter->second.NegativeButton = negativeButton;

	return true;
}
// IN: char* - Name of the binding to update
//     Axis  - Axis to update to
// OUT: bool - status of the update
//
// Updates the axis to the new button
bool InputManager::UpdateBoundAxis(std::string bindingName, Axis axis)
{
	auto iter = s_umKeyBindings.find(bindingName);
	if (iter == s_umKeyBindings.end())
		return false;

	iter->second.Axis = axis;

	return true;
}
// IN: char* - Name of the binding to update
//     bool  - update the inverse to
// OUT: bool - status of the update
//
// Updates the inverse bool to the new value
bool InputManager::UpdateBoundInverse(std::string bindingName, bool inverse)
{
	auto iter = s_umKeyBindings.find(bindingName);
	if (iter == s_umKeyBindings.end())
		return false;

	iter->second.Inverse = inverse;

	return true;
}

// IN: char* - Name of the binding to check
// OUT: bool - status of key pressed
//
// Checks all bound Keys and Buttons and returns true if any are pressed this frame
bool InputManager::BindingButtonPressed(std::string bindingName)
{
	auto iter = s_umKeyBindings.find(bindingName);
	if (iter == s_umKeyBindings.end())
		return false;

	InputCheck check = iter->second;

	if (IsMouseButtonPressed(check.PositiveMouseButton) || IsMouseButtonPressed(check.NegativeMouseButton))
	{
		s_bUsingController = false;
		return true;
	}

	if (IsKeyPressed(check.PositiveKey) || IsKeyPressed(check.NegativeKey))
	{
		s_bUsingController = false;
		return true;
	}

	if (IsButtonPressed(check.Controller, check.PositiveButton) || IsButtonPressed(check.Controller, check.NegativeButton))
	{
		s_bUsingController = true;
		return true;
	}

	return false;
}
// IN: char* - Name of the binding to check
// OUT: bool - status of key pressed
//
// Checks all bound inputs and returns true if any are down this frame
bool InputManager::BindingButtonDown(std::string bindingName)
{
	auto iter = s_umKeyBindings.find(bindingName);
	if (iter == s_umKeyBindings.end())
		return false;

	InputCheck check = iter->second;

	if (IsMouseButtonDown(check.PositiveMouseButton) || IsMouseButtonDown(check.NegativeMouseButton))
	{
		s_bUsingController = false;
		return true;
	}

	if (IsKeyDown(check.PositiveKey) || IsKeyDown(check.NegativeKey))
	{
		s_bUsingController = false;
		return true;
	}

	if (IsButtonDown(check.Controller, check.PositiveButton) || IsButtonDown(check.Controller, check.NegativeButton))
	{
		s_bUsingController = true;
		return true;
	}

	if (abs(GetAxis(check.Controller, check.Axis)) > 0.0f)
	{
		s_bUsingController = true;
		return true;
	}

	return false;
}
// IN: char* - Name of the binding to check
// OUT: bool - status of key pressed
//
// Checks all bound Keys and Buttons and returns true if any are released this frame
bool InputManager::BindingButtonReleased(std::string bindingName)
{
	auto iter = s_umKeyBindings.find(bindingName);
	if (iter == s_umKeyBindings.end())
		return false;

	InputCheck check = iter->second;

	if (IsMouseButtonReleased(check.PositiveMouseButton) || IsMouseButtonReleased(check.NegativeMouseButton))
	{
		s_bUsingController = false;
		return true;
	}

	if (IsKeyReleased(check.PositiveKey) || IsKeyReleased(check.NegativeKey))
	{
		s_bUsingController = false;
		return true;
	}

	if (IsButtonReleased(check.Controller, check.PositiveButton) || IsButtonReleased(check.Controller, check.NegativeButton))
	{
		s_bUsingController = true;
		return true;
	}

	return false;
}

// IN: char* - Name of the binding to check
// OUT: float - value of the axis
//
// Returns between a -1.0f to 1.0f based on the bound inputs. Buttons and Keys will return flat
// values where axis will return between values.
float InputManager::BindingAxisRaw(std::string bindingName)
{
	auto iter = s_umKeyBindings.find(bindingName);
	if (iter == s_umKeyBindings.end())
		return 0.0f;

	InputCheck check = iter->second;
	float axis = 0.0f;

	if (IsMouseButtonDown(check.PositiveMouseButton) || IsKeyDown(check.PositiveKey) || IsButtonDown(check.Controller, check.PositiveButton))
		axis = 1.0f;
	else if (IsMouseButtonDown(check.NegativeMouseButton) || IsKeyDown(check.NegativeKey) || IsButtonDown(check.Controller, check.NegativeButton))
		axis = -1.0f;
	else
		axis = GetAxis(check.Controller, check.Axis);

	if (check.Inverse)
		axis = -axis;

	return axis;
}

// IN: char* - Name of binding to remove
// OUT: bool - Status of unbinding
//
// Removes the binding if it exists
bool InputManager::UnregisterBinding(std::string bindingName)
{
	auto iter = s_umKeyBindings.find(bindingName);
	if (iter == s_umKeyBindings.end())
		return false;

	s_umKeyBindings.erase(iter);
	return true;
}

// IN: Key - Key to unregister
// OUT: void
//
// Unregisters the key if it is registered
void InputManager::UnregisterKey(Key key)
{
	auto iter = s_setRegisteredKeys.find((int)key);

	if (iter != s_setRegisteredKeys.end())
		s_setRegisteredKeys.erase(iter);
}

// IN: HWND - handle to the window that will be used by the input manager.
//            Only needed for centering purposes so nullptr can be passed and it will use the desktop
// OUT: bool - States if the manager set up correctly
//
// Initializes the input manager
bool InputManager::Initialize(HWND window)
{
	// Sets all our keys to 0 by default
	for (size_t index = 0; index < 256; s_keys[index++] = 0);

	s_hWindow = window;
	s_CenterMouse = false;
	s_CheckAllKeys = false;
	GetCursorPos(&s_prevMousePosition);
	s_mouseMovement.x = s_mouseMovement.y = 0;
	s_iMouseWheel = s_iMouseWheelChange = 0;

	// Do the initial check to see which controllers are connected
	DWORD dwResult;
	for (DWORD i = 0; i< XUSER_MAX_COUNT; i++)
	{
		ZeroMemory(&s_ControllerInputStates[i], sizeof(XINPUT_STATE));

		// Simply get the state of the controller from XInput.
		dwResult = XInputGetState(i, &s_ControllerInputStates[i]);

		if (dwResult == ERROR_SUCCESS)
		{
			// Controller is connected
			s_ConnectedControllers.SetFlag(0, true);

			std::string print = "Controller " + std::to_string(i + 1) + " Connected!\n";
			PRINT(print.c_str());
		}
	}

	return true;
}

// IN: void
// OUT: bool - Status of if the controller is being used
//
// Returns the status of the use of the controller
bool InputManager::UsingController() { return s_bUsingController; }

// IN: void
// OUT: bool - States if the manager updated successfully
//
// Updates the input manager from all changes from last frame
bool InputManager::Update()
{
	// Create a static timer that will keep track of when we can perform a check for newly connected controllers
	static InfernoTimer s_CheckTimer(CONTROLLER_CONNECTION_CHECK, true);

	if (s_bPrevUsingController != s_bUsingController)
	{
		s_pDelegateRegistry.InvokeControlChangeEvent(s_bUsingController);
		s_bPrevUsingController = s_bUsingController;
	}

	POINT currMousePos;
	GetCursorPos(&currMousePos);
	currMousePos.x -= s_WindowPosition.x;
	currMousePos.y -= s_WindowPosition.y;
	s_mouseMovement = { currMousePos.x - s_prevMousePosition.x, currMousePos.y - s_prevMousePosition.y };

	if (s_CenterMouse && s_hWindow != nullptr && !s_bLostFocus)
	{
		RECT WindowRect;
		GetWindowRect(s_hWindow, &WindowRect);
		SetCursorPos((int)((WindowRect.left + WindowRect.right) * 0.5f), (int)((WindowRect.top + WindowRect.bottom) * 0.5f));
		GetCursorPos(&s_prevMousePosition);
		s_prevMousePosition.x -= s_WindowPosition.x;
		s_prevMousePosition.y -= s_WindowPosition.y;
	}
	else
		s_prevMousePosition = currMousePos;

	// Check to see if there was mouse movement this frame and that we have window focus, and if so send out an event
	if ((s_mouseMovement.x != 0 || s_mouseMovement.y != 0) && !s_bLostFocus)
		s_pDelegateRegistry.InvokeMouseMovementEvent(s_prevMousePosition, s_mouseMovement);

	// Update the mouse buttons
	for (size_t mouseButton = 0; mouseButton < TOTAL_MOUSE_BUTTONS; mouseButton++)
	{
		int shift = 1 << mouseButton;
		int flag = (s_cMouseButtons & shift) >> mouseButton;
		s_cMouseButtonStates[mouseButton] = ((s_cMouseButtonStates[mouseButton] & KEY_PRESSED) >> 1) | (flag << 7);
	}

	// Update the Mouse Wheel direction to what it was this frame
	s_iMouseWheel = s_iMouseWheelChange;
	// Reset the change to 0
	s_iMouseWheelChange = 0;

	if (s_iMouseWheel != 0)
		s_pDelegateRegistry.InvokeMouseWheelEvent(s_iMouseWheel);

	// Checks the previous state of the key to see if it had been pressed, and if so shift the bit over by one
	// then check to see if the key is still down by or'ing in the return from GetKeyboardState
	// By doing this it will state per frame if the key is Pressed, Down, or Released
#define SETKEY(key) (s_keys[key] = (s_keys[key] & KEY_PRESSED) >> 1 | s_changes[key])
	
	if (!s_CheckAllKeys)
	{
		// Only update registered keys
		for (auto iter = s_setRegisteredKeys.begin(); iter != s_setRegisteredKeys.end(); ++iter)
			SETKEY(*iter);
	}
	else
	{
		for (size_t key = 0; key < 256; key++)
			SETKEY(key);
	}

#undef SETKEY

	// Check all connected controllers
	DWORD dwResult;
	for (DWORD i = 0; i < XUSER_MAX_COUNT; i++)
	{
		// If the controller isn't connected, check next controller
		if (!s_ConnectedControllers.GetFlag(i))
			continue;

		// Simply get the state of the controller from XInput.
		dwResult = XInputGetState(i, &s_ControllerInputStates[i]);

		if (dwResult == ERROR_SUCCESS)
		{
			// Update our buttons if we are still connected
			for (DWORD button = 0; button < TOTAL_BUTTONS; button++)
			{
				int shift = 1 << button;
				int flag = (s_ControllerInputStates[i].Gamepad.wButtons & shift) >> button;
				s_buttonStates[i][button] = ((s_buttonStates[i][button] & KEY_PRESSED) >> 1) | (flag << 7);
			}

			if (abs(s_ControllerInputStates[i].Gamepad.sThumbLX) > s_ThumbDeadZone)
			{
				if (s_ControllerInputStates[i].Gamepad.sThumbLX >= 0)
					s_axisValues[i][(int)Axis::LeftStick_X] = float(s_ControllerInputStates[i].Gamepad.sThumbLX - s_ThumbDeadZone) / (MAX_DEADZONE - s_ThumbDeadZone);
				else
					s_axisValues[i][(int)Axis::LeftStick_X] = float(s_ControllerInputStates[i].Gamepad.sThumbLX + s_ThumbDeadZone) / (MAX_DEADZONE_NEG - s_ThumbDeadZone);
			}
			else
				s_axisValues[i][(int)Axis::LeftStick_X] = 0.0f;

			if (abs(s_ControllerInputStates[i].Gamepad.sThumbLY) > s_ThumbDeadZone)
			{
				if (s_ControllerInputStates[i].Gamepad.sThumbLY >= 0)
					s_axisValues[i][(int)Axis::LeftStick_Y] = float(s_ControllerInputStates[i].Gamepad.sThumbLY - s_ThumbDeadZone) / (MAX_DEADZONE - s_ThumbDeadZone);
				else
					s_axisValues[i][(int)Axis::LeftStick_Y] = float(s_ControllerInputStates[i].Gamepad.sThumbLY + s_ThumbDeadZone) / (MAX_DEADZONE_NEG - s_ThumbDeadZone);
			}
			else
				s_axisValues[i][(int)Axis::LeftStick_Y] = 0.0f;

			if (abs(s_ControllerInputStates[i].Gamepad.sThumbRX) > s_ThumbDeadZone)
			{
				if (s_ControllerInputStates[i].Gamepad.sThumbRX >= 0)
					s_axisValues[i][(int)Axis::RightStick_X] = float(s_ControllerInputStates[i].Gamepad.sThumbRX - s_ThumbDeadZone) / (MAX_DEADZONE - s_ThumbDeadZone);
				else
					s_axisValues[i][(int)Axis::RightStick_X] = float(s_ControllerInputStates[i].Gamepad.sThumbRX + s_ThumbDeadZone) / (MAX_DEADZONE_NEG - s_ThumbDeadZone);
			}
			else
				s_axisValues[i][(int)Axis::RightStick_X] = 0.0f;

			if (abs(s_ControllerInputStates[i].Gamepad.sThumbRY) > s_ThumbDeadZone)
			{
				if (s_ControllerInputStates[i].Gamepad.sThumbRY >= 0)
					s_axisValues[i][(int)Axis::RightStick_Y] = float(s_ControllerInputStates[i].Gamepad.sThumbRY - s_ThumbDeadZone) / (MAX_DEADZONE - s_ThumbDeadZone);
				else
					s_axisValues[i][(int)Axis::RightStick_Y] = float(s_ControllerInputStates[i].Gamepad.sThumbRY + s_ThumbDeadZone) / (MAX_DEADZONE_NEG - s_ThumbDeadZone);
			}
			else
				s_axisValues[i][(int)Axis::RightStick_Y] = 0.0f;

			if (s_ControllerInputStates[i].Gamepad.bLeftTrigger > s_TriggerThreshold)
				s_axisValues[i][(int)Axis::LeftTrigger] = float(s_ControllerInputStates[i].Gamepad.bLeftTrigger - s_TriggerThreshold) / (MAX_THRESHOLD - s_TriggerThreshold);
			else
				s_axisValues[i][(int)Axis::LeftTrigger] = 0.0f;

			if (s_ControllerInputStates[i].Gamepad.bRightTrigger > s_TriggerThreshold)
				s_axisValues[i][(int)Axis::RightTrigger] = float(s_ControllerInputStates[i].Gamepad.bRightTrigger - s_TriggerThreshold) / (MAX_THRESHOLD - s_TriggerThreshold);
			else
				s_axisValues[i][(int)Axis::RightTrigger] = 0.0f;
		}
		else
		{
			// Set the controller as disconnected
			s_ConnectedControllers.SetFlag(i, false);
			s_pDelegateRegistry.InvokeControllerLostEvent(i);

			std::string print = "Controller " + std::to_string(i + 1) + " Disconnected!\n";
			PRINT(print.c_str());
		}
	}

	// Check for newly connected controllers if enough time has passed
	if (s_CheckTimer.IsFinished())
	{
		for (DWORD i = 0; i < XUSER_MAX_COUNT; i++)
		{
			// Continue if the controller is already connected
			if (s_ConnectedControllers.GetFlag(i))
				continue;

			// Simply get the state of the controller from XInput.
			dwResult = XInputGetState(i, &s_ControllerInputStates[i]);

			if (dwResult == ERROR_SUCCESS)
			{
				// Set the controller as connected
				s_ConnectedControllers.SetFlag(i, true);
				s_pDelegateRegistry.InvokeControllerGainedEvent(i);

				std::string print = "Controller " + std::to_string(i + 1) + " Connected!\n";
				PRINT(print.c_str());
			}
		}

		s_CheckTimer.Reset();
	}

	// Loop through all registered bindings after our controllers update so we get a more accurate axis information
	auto iter = s_umKeyBindings.begin();
	for (; iter != s_umKeyBindings.end(); ++iter)
	{
		if (BindingButtonPressed(iter->first))
		{
			s_pDelegateRegistry.InvokeActionEvent(iter->first.c_str(), ACTION_STATUS::AS_PRESSED);
			s_pDelegateRegistry.InvokeActionEvent(iter->first.c_str(), ACTION_STATUS::AS_DOWN);
			s_pDelegateRegistry.InvokeAxisEvent(iter->first.c_str(), BindingAxisRaw(iter->first));
		}
		else if (BindingButtonDown(iter->first))
		{
			s_pDelegateRegistry.InvokeActionEvent(iter->first.c_str(), ACTION_STATUS::AS_DOWN);
			s_pDelegateRegistry.InvokeAxisEvent(iter->first.c_str(), BindingAxisRaw(iter->first));
		}
		else if (BindingButtonReleased(iter->first))
		{
			s_pDelegateRegistry.InvokeActionEvent(iter->first.c_str(), ACTION_STATUS::AS_RELEASED);
			s_pDelegateRegistry.InvokeAxisEvent(iter->first.c_str(), BindingAxisRaw(iter->first));
		}
	}

	return true;
}

// IN: Button       - Button to check
// OUT: bool - Status of the button
//
// Checks the button to see if it was pressed this frame
bool InputManager::IsMouseButtonPressed(Button button)
{
	if (button == Button::None)
		return false;

	return (s_cMouseButtonStates[(int)button] & KEY_MASK) == KEY_PRESSED;
}
// IN: Button       - Button to check
// OUT: bool - Status of the button
//
// Checks the button to see if it is down
bool InputManager::IsMouseButtonDown(Button button)
{
	if (button == Button::None)
		return false;

	return (s_cMouseButtonStates[(int)button] & KEY_DOWN) == KEY_DOWN;
}
// IN: Button       - Button to check
// OUT: bool - Status of the button
//
// Checks the button to see if it was released this frame
bool InputManager::IsMouseButtonReleased(Button button)
{
	if (button == Button::None)
		return false;

	return (s_cMouseButtonStates[(int)button] & KEY_MASK) == KEY_RELEASED;
}

// IN: void
// OUT: bool - States if any button was pressed
//
// Checks to see if any button has been pressed this frame
bool InputManager::IsAnyMouseButtonPressed()
{
	for (size_t button = 0; button < TOTAL_MOUSE_BUTTONS; button++)
	{
		if ((s_cMouseButtonStates[button] & KEY_MASK) == KEY_PRESSED)
			return true;
	}

	return false;
}
// IN: void
// OUT: bool - States if any button is down
//
// Checks to see if any button is currently down
bool InputManager::IsAnyMouseButtonDown()
{
	for (size_t button = 0; button < TOTAL_MOUSE_BUTTONS; button++)
	{
		if ((s_cMouseButtonStates[button] & KEY_DOWN) == KEY_DOWN)
			return true;
	}

	return false;
}
// IN: void
// OUT: bool - States if any button was released
//
// Checks to see if any button has been released this frame
bool InputManager::IsAnyMouseButtonReleased()
{
	for (size_t button = 0; button < TOTAL_MOUSE_BUTTONS; button++)
	{
		if ((s_cMouseButtonStates[button] & KEY_MASK) == KEY_RELEASED)
			return true;
	}

	return false;
}

// IN: Key - Key to check
// OUT: bool - Status of the key
//
// Checks the key to see if it was pressed this frame
bool InputManager::IsKeyPressed(Key key){ return (s_keys[(int)key] & KEY_MASK) == KEY_PRESSED; }
// IN: Key - Key to check
// OUT: bool - Status of the key
//
// Checks the key to see if it is down
bool InputManager::IsKeyDown(Key key) { return (s_keys[(int)key] & KEY_DOWN) == KEY_DOWN; }
// IN: Key - Key to check
// OUT: bool - Status of the key
//
// Checks the key to see if it was released this frame
bool InputManager::IsKeyReleased(Key key) { return (s_keys[(int)key] & KEY_MASK) == KEY_RELEASED; }

// IN: void
// OUT: bool - States if any key was pressed
//
// Checks to see if any key has been pressed this frame
bool InputManager::IsAnyKeyPressed()
{
	for (size_t key = 1; key < 256; key++)
	{
		if ((s_keys[key] & KEY_MASK) == KEY_PRESSED)
			return true;
	}

	return false;
}

// IN: void
// OUT: bool - States if any key is down
//
// Checks to see if any key is currently down
bool InputManager::IsAnyKeyDown()
{
	for (size_t key = 1; key < 256; key++)
	{
		if ((s_keys[key] & KEY_DOWN) == KEY_DOWN)
			return true;
	}

	return false;
}

// IN: void
// OUT: bool - States if any key was released
//
// Checks to see if any key has been released this frame
bool InputManager::IsAnyKeyReleased()
{
	for (size_t key = 1; key < 256; key++)
	{
		if ((s_keys[key] & KEY_MASK) == KEY_RELEASED)
			return true;
	}

	return false;
}

// IN: Button - Button to check
// OUT: bool - Status of the button
//
// Checks the button to see if it was pressed this frame
bool InputManager::IsButtonPressed(unsigned int controller, Button button)
{
	if (controller > XUSER_MAX_COUNT || !s_ConnectedControllers.GetFlag(controller) || button == Button::None)
		return false;

	return (s_buttonStates[controller][(int)button] & KEY_MASK) == KEY_PRESSED;
}
// IN: Button - Button to check
// OUT: bool - Status of the button
//
// Checks the button to see if it is down
bool InputManager::IsButtonDown(unsigned int controller, Button button)
{
	if (controller > XUSER_MAX_COUNT || !s_ConnectedControllers.GetFlag(controller) || button == Button::None)
		return false;

	return (s_buttonStates[controller][(int)button] & KEY_DOWN) == KEY_DOWN;
}
// IN: Button - Button to check
// OUT: bool - Status of the button
//
// Checks the button to see if it was released this frame
bool InputManager::IsButtonReleased(unsigned int controller, Button button)
{
	if (controller > XUSER_MAX_COUNT || !s_ConnectedControllers.GetFlag(controller) || button == Button::None)
		return false;

	return (s_buttonStates[controller][(int)button] & KEY_MASK) == KEY_RELEASED;
}

// IN: void
// OUT: bool - States if any button was pressed
//
// Checks to see if any button has been pressed this frame
bool InputManager::IsAnyButtonPressed(unsigned int controller)
{
	if (controller > XUSER_MAX_COUNT || !s_ConnectedControllers.GetFlag(controller))
		return false;

	for (DWORD button = 0; button < TOTAL_BUTTONS; button++)
	{
		if ((s_buttonStates[controller][button] & KEY_MASK) == KEY_PRESSED)
			return true;
	}

	return false;
}
// IN: void
// OUT: bool - States if any button is down
//
// Checks to see if any button is currently down
bool InputManager::IsAnyButtonDown(unsigned int controller)
{
	if (controller > XUSER_MAX_COUNT || !s_ConnectedControllers.GetFlag(controller))
		return false;

	for (DWORD button = 0; button < TOTAL_BUTTONS; button++)
	{
		if ((s_buttonStates[controller][button] & KEY_DOWN) == KEY_DOWN)
			return true;
	}

	return false;
}
// IN: void
// OUT: bool - States if any button was released
//
// Checks to see if any button has been released this frame
bool InputManager::IsAnyButtonReleased(unsigned int controller)
{
	if (controller > XUSER_MAX_COUNT || !s_ConnectedControllers.GetFlag(controller))
		return false;

	for (DWORD button = 0; button < TOTAL_BUTTONS; button++)
	{
		if ((s_buttonStates[controller][button] & KEY_MASK) == KEY_RELEASED)
			return true;
	}

	return false;
}

// IN: unsigned int - Controller to check
//     Axis         - Axis to check
// OUT: int - Range of the axis
//
// Checks the axis of the controller specified and returns the value
float InputManager::GetAxis(unsigned int controller, Axis axis)
{
	if (controller > XUSER_MAX_COUNT || !s_ConnectedControllers.GetFlag(controller) || axis == Axis::None)
		return false;

	return s_axisValues[controller][(int)axis];
}

// IN: void
// OUT: int - MouseWheel Direction
//
// Returns the direction of the mouse wheel this frame
// 1 - Wheel Up, 0 - No Movement, -1 - Wheel Down
int InputManager::GetMouseWheel()
{
	return s_iMouseWheel;
}

// IN: HWND - Window to use by the manager
// OUT: void
//
// Sets the incoming window as the window to use for centering the mouse
void InputManager::SetWindow(HWND window) { s_hWindow = window; }

// IN: SHORT - DeadZone to use
// OUT: void
//
// Sets the deadzone to the specified amount if in range
void InputManager::SetThumbDeadZone(SHORT deadzone)
{
	if (deadzone > MAX_DEADZONE)
		deadzone = MAX_DEADZONE;

	s_ThumbDeadZone = deadzone;
}

// IN: BYTE - Threshold to use
// OUT: void
//
// Sets the threshold to the specified amount if in range
void InputManager::SetTriggerThreshold(BYTE threshold)
{
	if (threshold > MAX_THRESHOLD)
		threshold = MAX_THRESHOLD;

	s_TriggerThreshold = threshold;
}

// IN: bool - States the visibilty of the mouse
// OUT: void
//
// Sets the mouse's visibility based on the incoming bool
void InputManager::ShowMouse(bool visible)
{
	if (!s_bLostFocus)
		s_bShowMouse = visible;

	if (visible)
		while (ShowCursor(visible) < 0);
	else
		while (ShowCursor(visible) >= 0);
}

// IN: void
// OUT: POINT - Movement made by the mouse
//
// Returns the total movement the mouse has made since last frame
POINT InputManager::GetMouseMovement() { return s_mouseMovement; }
// IN: void
// OUT: POINT - Current mouse position on screen
//
// Returns the current mouse position on screen
POINT InputManager::GetMousePosition() { return s_prevMousePosition; }

// IN : char* - The name of the binding
//      ActionEventFunction - The delegate function to register to the binding
// OUT : void
//
// Registers a delegate function to a specific Action, that will be called when there's an update for the Action.
void InputManager::RegisterEventToActionBinding(const char* _binding, void* _callingObject, ActionEventFunction _delegate)
{
	s_pDelegateRegistry.RegisterEventToActionBinding(_binding, _callingObject, _delegate);
}

// IN : char* - The name of the binding
//      AxisEventFunction - The delegate function to register to the binding
// OUT : void
//
// Registers a delegate function to a specific Axis, that will be called when there's an update for that Axis.
void InputManager::RegisterEventToAxisBinding(const char* _binding, void* _callingObject, AxisEventFunction _delegate)
{
	s_pDelegateRegistry.RegisterEventToAxisBinding(_binding, _callingObject, _delegate);
}

// IN : MouseMovementEventFunction - The delegate to register to MouseMovement Event.
// OUT : void
// 
// Registers a delegate to the MouseMovement Event, that will be called anytime the mouse moves.
void InputManager::RegisterEventToMouseMovement(void* _callingObject, MouseMovementEventFunction _delegate)
{
	s_pDelegateRegistry.RegisterEventToMouseMovement(_callingObject, _delegate);
}

// IN : MouseWheelEventFunction - The delegate to register to MouseWheel Event.
// OUT : void
// 
// Registers a delegate to the MouseWheel Event, that will be called anytime the mouse moves.
void InputManager::RegisterEventToMouseWheel(void* _callingObject, MouseWheelEventFunction _delegate)
{
	s_pDelegateRegistry.RegisterEventToMouseWheel(_callingObject, _delegate);
}

// IN : ControlChangeEventFunction - The delegate to register to ControlChange Event
// OUT : void
//
// Registers a delegate to the ControlChange Event, that will be called anytime the control changes between mouse and controller
void InputManager::RegisterEventToControlChange(void* _callingObject, ControlChangeEventFunction _delegate)
{
	s_pDelegateRegistry.RegisterEventToControlChange(_callingObject, _delegate);
}

// IN : ControllerLostEventFunction - The delegate to register to ControllerLost Event
// OUT : void
//
// Registers a delegate to the ControllerLost Event, that will be called if a controller is unplugged
void InputManager::RegisterEventToControllerLost(void* _callingObject, ControllerLostEventFunction _delegate)
{
	s_pDelegateRegistry.RegisterEventToControllerLost(_callingObject, _delegate);
}

// IN : ControllerGainedEventFunction - The delegate to register to ControllerGained Event
// OUT : void
//
// Registers a delegate to the ControllerGained Event, that will be called if a controller is plugged in
void InputManager::RegisterEventToControllerGained(void* _callingObject, ControllerGainedEventFunction _delegate)
{
	s_pDelegateRegistry.RegisterEventToControllerGained(_callingObject, _delegate);
}

// IN : char* - The name of the binding
//      ActionEventFunction - The delegate function to unregister
// OUT : void
//
// Unregisters a delegate function from a specific Action.
bool InputManager::UnregisterEventFromAction(const char* _binding, void* _callingObject, ActionEventFunction _delegate)
{
	return s_pDelegateRegistry.UnregisterEventFromActionBinding(_binding, _callingObject, _delegate);
}

// IN : char* - The name of the binding
//      ActionEventFunction - The delegate function to unregister
// OUT : void
//
// Unregisters a delegate function from a specific Action.
bool InputManager::UnregisterEventFromAction(unsigned int _hash, void* _callingObject, ActionEventFunction _delegate)
{
	return s_pDelegateRegistry.UnregisterEventFromActionBinding(_hash, _callingObject, _delegate);
}

// IN : char* - The name of the binding
//      AxisEventFunction - The delegate function to unregister
// OUT : void
//
// Unregisters a delegate function from a specific Axis.
bool InputManager::UnregisterEventFromAxis(const char* _binding, void* _callingObject, AxisEventFunction _delegate)
{
	return s_pDelegateRegistry.UnregisterEventFromAxisBinding(_binding, _callingObject, _delegate);
}

// IN : char* - The name of the binding
//      AxisEventFunction - The delegate function to unregister
// OUT : void
//
// Unregisters a delegate function from a specific Axis.
bool InputManager::UnregisterEventFromAxis(unsigned int _hash, void* _callingObject, AxisEventFunction _delegate)
{
	return s_pDelegateRegistry.UnregisterEventFromAxisBinding(_hash, _callingObject, _delegate);
}

// IN : MouseMovementEventFunction - The delegate function to unregister
// OUT : void
//
// Unregisters a delegate function from a specific Action.
bool InputManager::UnregisterEventFromMouseMovement(void* _callingObject, MouseMovementEventFunction _delegate)
{
	return s_pDelegateRegistry.UnregisterEventFromMouseMovement(_callingObject, _delegate);
}

// IN :  - The delegate function to unregister
// OUT : void
//
// Unregisters a delegate function from a specific Action.
bool InputManager::UnregisterEventFromMouseWheel(void* _callingObject, MouseWheelEventFunction _delegate)
{
	return s_pDelegateRegistry.UnregisterEventFromMouseWheel(_callingObject, _delegate);
}

// IN : ControlChangeEventFunction - The delegate function to unregister
// OUT : void
//
// Unregisters a delegate function from a specific Action.
bool InputManager::UnregisterEventFromControlChange(void* _callingObject, ControlChangeEventFunction _delegate)
{
	return s_pDelegateRegistry.UnregisterEventFromControlChange(_callingObject, _delegate);
}

// IN : ControllerLostEventFunction - The delegate function to unregister
// OUT : void
//
// Unregisters a delegate function from a specific Action.
bool InputManager::UnregisterEventFromControllerLost(void* _callingObject, ControllerLostEventFunction _delegate)
{
	return s_pDelegateRegistry.UnregisterEventFromControllerLost(_callingObject, _delegate);
}

// IN : ControllerGainedEventFunction - The delegate function to unregister
// OUT : void
//
// Unregisters a delegate function from a specific Action.
bool InputManager::UnregisterEventFromControllerGained(void* _callingObject, ControllerGainedEventFunction _delegate)
{
	return s_pDelegateRegistry.UnregisterEventFromControllerGained(_callingObject, _delegate);
}