// Author : Jack Gillette
//
// Holds all the enum classes that are needed when working with Input; Event Types and Status'
#pragma once

#include <functional>

struct tagPOINT;

// === Defines
#define BindActionFunction(Function, Class) { std::bind(Function, Class, std::placeholders::_1) }
#define BindAxisFunction(Function, Class) { std::bind(Function, Class, std::placeholders::_1) }
#define BindMouseMovementFunction(Function, Class) { std::bind(Function, Class, std::placeholders::_1, std::placeholders::_2) }
#define BindControlChangeFunction(Function, Class) { std::bind(Function, Class, std::placeholders::_1) }
#define BindControllerLostFunction(Function, Class) { std::bind(Function, Class, std::placeholders::_1) }
#define BindControllerGainedFunction(Function, Class) { std::bind(Function, Class, std::placeholders::_1) }

// === Enumerations
enum class INPUT_EVENTS {
	IE_ACTION = 0,
	IE_AXIS = 1,
	IE_MOUSEMOVEMENT = 2,
	IE_MAX_EVENTS = 3
};

enum class ACTION_STATUS {
	AS_PRESSED = 0,
	AS_DOWN = 1,
	AS_RELEASED = 2
};

// === TypeDefs
typedef std::function<void(ACTION_STATUS)> ActionEventFunction;
typedef std::function<void(float)> AxisEventFunction;
typedef std::function<void(tagPOINT, tagPOINT)> MouseMovementEventFunction;
typedef std::function<void(unsigned int)> MouseWheelEventFunction;
typedef std::function<void(bool)> ControlChangeEventFunction;
typedef std::function<void(unsigned int)> ControllerLostEventFunction;
typedef std::function<void(unsigned int)> ControllerGainedEventFunction;