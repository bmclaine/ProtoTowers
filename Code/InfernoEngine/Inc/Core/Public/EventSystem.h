// Author : Jack Gillette
//
// Allows class to register for Events, which can be called and passed along with data from other classes, without knowing
// about any of the listeners.
#pragma once

#include <functional>
#include <queue>
#include <unordered_map>
#include <vector>

// === Defines
#define BindEventFunction(Function, Class) { std::bind(Function, Class, std::placeholders::_1) }

class EventSystem
{
private:
	// === TypeDef
	typedef std::function<void(void*)> EventFunction;

	// === Structures
	struct EventData {
		unsigned int ID;
		void* pData;
	};

	struct EventDelegate {
		void* CallingObject;
		EventFunction Delegate;

		EventDelegate(void* _callingObject, EventFunction _delegate) {
			CallingObject = _callingObject;
			Delegate = _delegate;
		}
	};

	// === TypeDef
	typedef std::unordered_map<unsigned int, std::vector<EventDelegate>>::iterator EventIter;

	// === Variables
	std::unordered_map<unsigned int, std::vector<EventDelegate>> m_RegisteredEvents;
	std::queue<EventData> m_QueuedEvents;

	// === Static Variables
	static EventSystem* s_pInstance;

	// === Singleton === //
	EventSystem() = default;
	EventSystem(const EventSystem& _other) = delete;
	EventSystem& operator=(const EventSystem& _rhs) = delete;
	~EventSystem() = default;

public:
	// === Interface === //

	// === Shutdown
	//  IN : void
	//  OUT : void
	//
	//  Handles shutting down the EventSystem instance, cleaning up any events that may still be queued.
	void Shutdown();
	// === Update
	//  IN : void
	//  OUT : void
	//
	//  Goes through any Queued Events and sends them off
	void Update();
	// === RegisterForEvent
	//  IN : char* - The ID of the event to register for
	//		 EventFunction - The Function that the event should trigger. { void Func(void*) }
	//  OUT : void
	//
	//  Registers a given function to an event. If the event already has been registered, it just adds the function to array of 
	//  functions to call, if not, it inserts it into the map of RegisteredFunctions.
	unsigned int RegisterForEvent(char* _eventID, void* _callingObject, EventFunction _function);
	// === UnregisterFromEvent
	//  IN : char* - The Event ID
	//       EventFunction - The delegate to unregister
	//  OUT : bool - Whether or not the delegate was found and unregistered
	//
	//  Unregisters the given delegate from a specific event. Returns true if it was found and removed, false otherwise.
	bool UnregisterFromEvent(char* _eventID, void* _callingObject, EventFunction _function);
	// === UnregisterFromEvent
	//  IN : unsigned int - The hash key of the Event
	//       EventFunction - The delegate to unregister
	//  OUT : bool - Whether or not the delegate was found and unregistered
	//
	//  Unregisters the given delegate from a specific event. Returns true if it was found and removed, false otherwise.
	bool UnregisterFromEvent(unsigned int _hash, void* _callingObject, EventFunction _function);
	// === QueueEvent
	//  IN : char* - Event ID
	//       void* - Pointer to the Data that needs to be sent with the Event. Note, this should be an actual pointer to dynamic memory.
	//               It will be cleaned up by the event system once it is used.
	//  OUT : void
	//
	//  Queues up an Event to be sent to any listeners it has. The Data that is passed in will need to be a pointer to actual dynamic
	//  memory, as the event will not be sent off right away. That memory how ever will be cleaned up by the EventSystem once it uses it.
	void QueueEvent(char* _eventID, void* _pData);
	// === QueueEvent
	//  IN : unsigned int - Event ID hash key
	//       void* - Pointer to the Data that needs to be sent with the Event. Note, this should be an actual pointer to dynamic memory.
	//               It will be cleaned up by the event system once it is used.
	//  OUT : void
	//
	//  Queues up an Event to be sent to any listeners it has. The Data that is passed in will need to be a pointer to actual dynamic
	//  memory, as the event will not be sent off right away. That memory how ever will be cleaned up by the EventSystem once it uses it.
	void QueueEvent(unsigned int _eventID, void* _pData);
	// === SendEVentNow
	//  IN : char* - The Event ID.
	//       void* - Pointer to the data that needs to be sent with the event. This memory, if dynamic, will not be cleaned up by the
	//               EventSystem.
	//  OUT : void
	//
	//  Sends an Event to all of its' listeners right away. Does not clean up the memory of the data. 
	void SendEventNow(char* _eventID, void* _pData);
	// === SendEVentNow
	//  IN : unsigned int - The Event ID hash key.
	//       void* - Pointer to the data that needs to be sent with the event. This memory, if dynamic, will not be cleaned up by the
	//               EventSystem.
	//  OUT : void
	//
	//  Sends an Event to all of its' listeners right away. Does not clean up the memory of the data. 
	void SendEventNow(unsigned int _eventID, void* _pData);
	// ================= //

	// === Static Interface === //

	// === GetInstance
	//  IN : void
	//  OUT : EventSystem* - Pointer to the static Instance of the EventSystem
	//
	//  Returns the static Instance of the EventSystem. If the EventSystem hasn't been created yet, it creates it, then returns it.
	static EventSystem* GetInstance();
	// === DeleteInstance
	// IN : void
	// OUT : void
	//
	// Deletes the current instance
	static void DeleteInstance();

	// ======================== //
};