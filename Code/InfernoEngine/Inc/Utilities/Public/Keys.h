// Author: Jonathan Gyurkovics
//
// An enum class that holds a list of most of the keyboard buttons

#pragma once

enum class Key
{
	None = 0,

	MouseLeft = 1, LButton = 1,
	MouseRight = 2, RButton = 2,
	Break = 3, Cancel = 3,
	MouseMiddle = 4, MButton = 4,
	MouseX1 = 5, XButton1 = 5,
	MouseX2 = 6, XButton2 = 6,
	Backspace = 8, Back = 8,
	Tab = 9,
	Clear = 12,
	Enter = 13, Return = 13,
	Shift = 16,								// Either Shift Key
	Control = 17,							// Either Ctrl Key
	Alt = 18, Menu = 18,					// Either Alt / Menu Key
	Pause = 19,
	CapsLock = 20, Capital = 20,
	Escape = 27,
	Spacebar = 32, Space = 32,
	PageUp = 33, Prior = 33,
	PageDown = 34, Next = 34,
	End = 35,
	Home = 36,
	LeftArrow = 37, Left = 37,
	UpArrow = 38, Up = 38,
	RightArrow = 39, Right = 39,
	DownArrow = 40, Down = 40,
	Select = 41,
	Print = 42,
	Execute = 43,
	PrintScreen = 44, Snapshot = 44,
	Insert = 45,
	Delete = 46,
	N0 = 48, Zero = 48,						// Number 0
	N1 = 49, One = 49,						// Number 1
	N2 = 50, Two = 50,						// Number 2
	N3 = 51, Three = 51,					// Number 3
	N4 = 52, Four = 52,						// Number 4
	N5 = 53, Five = 53,						// Number 5
	N6 = 54, Six = 54,						// Number 6
	N7 = 55, Seven = 55,					// Number 7
	N8 = 56, Eight = 56,					// Number 8
	N9 = 57, Nine = 57,						// Number 9
	A = 65,
	B = 66,
	C = 67,
	D = 68,
	E = 69,
	F = 70,
	G = 71,
	H = 72,
	I = 73,
	J = 74,
	K = 75,
	L = 76,
	M = 77,
	N = 78,
	O = 79,
	P = 80,
	Q = 81,
	R = 82,
	S = 83,
	T = 84,
	U = 85,
	V = 86,
	W = 87,
	X = 88,
	Y = 89,
	Z = 90,
	LeftWindows = 91,
	RightWindows = 92,
	NumPad0 = 96,
	NumPad1 = 97,
	NumPad2 = 98,
	NumPad3 = 99,
	NumPad4 = 100,
	NumPad5 = 101,
	NumPad6 = 102,
	NumPad7 = 103,
	NumPad8 = 104,
	NumPad9 = 105,
	Multiply = 106,
	Add = 107,
	Separator = 108,
	Subtract = 109,
	Decimal = 110,
	Divide = 111,
	F1 = 112,
	F2 = 113,
	F3 = 114,
	F4 = 115,
	F5 = 116,
	F6 = 117,
	F7 = 118,
	F8 = 119,
	F9 = 120,
	F10 = 121,
	F11 = 122,
	F12 = 123,
	NumLock = 144,
	ScrollLock = 145, Scroll = 145,
	LeftShift = 160,
	RightShift = 161,
	LeftControl = 162,
	RightControl = 163,
	LeftAlt = 164, LeftMenu = 164,
	RightAlt = 165, RightMenu = 165,
	Semicolon = 186, OemSemicolon = 186,
	Plus = 187, OemPlus = 187,
	Comma = 188, OemComma = 188,
	Minus = 189, OemMinus = 189,
	Period = 190, OemPeriod = 190,
	Question = 191, OemQuestion = 191,
	Tilde = 192, OemTilde = 192,
	OpenBracket = 219, OemOpenBracket = 219,
	Pipe = 220, OemPipe = 220,
	CloseBracket = 221, OemCloseBracket = 221,
	Quote = 222, OemQuote = 222,
	Backslash = 226, OemBackslash = 226,
};