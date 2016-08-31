// Author : Jack Gillette 
//
// Enum class that has the different types of events that can be triger on UIElements

#pragma once

enum class UI_EVENTS
{
	UI_EVENT_ONCLICK = 0,
	UI_EVENT_ONRELEASED = 1,
	UI_EVENT_ONHOVERBEGIN = 2,
	UI_EVENT_ONHOVEREND = 3,
	UI_EVENT_ONSELECT = 4,
	UI_EVENT_ONDESELECT = 5,
	UI_EVENT_ONDISABLED = 6,
	UI_EVENT_ONENABLED = 7,
	UI_EVENT_MAX = 8
};