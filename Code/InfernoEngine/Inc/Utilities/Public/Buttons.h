// Author: Jonathan Gyurkovics
//
// An enum class that holds a list of all the controller buttons and axis
#pragma once

#define TOTAL_BUTTONS 16
#define TOTAL_AXIS 6
#define TOTAL_MOUSE_BUTTONS 7

enum class Button
{
	DPad_Up = 0, LeftClick = 0,
	DPad_Down = 1, RightClick = 1,
	DPad_Left = 2, 
	DPad_Right = 3,
	Start = 4, MiddleClick = 4,
	Back = 5, MouseButton4 = 5,
	LeftStick = 6, MouseButton5 = 6,
	RightStick = 7,
	LeftBumper = 8,
	RightBumper = 9,
	A = 12,
	B = 13,
	X = 14,
	Y = 15,
	None = 255,
};

enum class Axis
{
	LeftStick_X = 0,
	LeftStick_Y = 1,
	RightStick_X = 2,
	RightStick_Y = 3,
	LeftTrigger = 4,
	RightTrigger = 5,
	None = 255,
};