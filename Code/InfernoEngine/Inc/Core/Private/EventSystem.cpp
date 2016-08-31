#include "EventSystem.h"

// === Static Variables
EventSystem* EventSystem::s_pInstance = nullptr;

// ===== Interface ===== //

// === Shutdown
//  IN : void
//  OUT : void
//
//  Handles shutting down the EventSystem instance, cleaning up any events that may still be queued.
void EventSystem::Shutdown()
{
	// === Delete any Events that might still be queued
	EventData currEvent;
	while (!m_QueuedEvents.empty()) {
		currEvent = m_QueuedEvents.front();
		
		if (currEvent.pData) delete currEvent.pData;

		m_QueuedEvents.pop();
	}
}

// === Update
//  IN : void
//  OUT : void
//
//  Goes through any Queued Events and sends them off
void EventSystem::Update()
{
	EventData currEvent;
	EventIter iter;

	while (!m_QueuedEvents.empty()) {
		currEvent = m_QueuedEvents.front();

		iter = m_RegisteredEvents.find(currEvent.ID);
		if (iter != m_RegisteredEvents.end()) {
			size_t count = iter->second.size();
			for (size_t i = 0; i < count; ++i) {
				iter->second[i].Delegate(currEvent.pData);
			}
		}

		if (currEvent.pData) delete currEvent.pData;
		m_QueuedEvents.pop();
	}
}

// === RegisterForEvent
//  IN : char* - The ID of the event to register for
//		 EventFunction - The Function that the event should trigger. { void Func(void*) }
//  OUT : void
//
//  Registers a given function to an event. If the event already has been registered, it just adds the function to array of 
//  functions to call, if not, it inserts it into the map of RegisteredFunctions.
unsigned int EventSystem::RegisterForEvent(char* _eventID, void* _callingObject, EventFunction _function)
{
	unsigned int hash = (unsigned int)std::hash<std::string>()(_eventID);

	EventIter iter = m_RegisteredEvents.find(hash);
	if (iter == m_RegisteredEvents.end()) {
		// === New Event, insert it
		m_RegisteredEvents.insert(std::make_pair(hash, std::vector<EventDelegate> { EventDelegate(_callingObject, _function) }));
	}
	else {
		// === Already added event
		// == Check for Duplicates
		size_t count = iter->second.size();
		for (size_t i = 0; i < count; ++i) {
			if (iter->second[i].CallingObject == _callingObject) {
				return hash;
			}
		}

		iter->second.push_back(EventDelegate(_callingObject, _function));
	}

	return hash;
}

// === UnregisterFromEvent
//  IN : char* - The Event ID
//       EventFunction - The delegate to unregister
//  OUT : bool - Whether or not the delegate was found and unregistered
//
//  Unregisters the given delegate from a specific event. Returns true if it was found and removed, false otherwise.

bool EventSystem::UnregisterFromEvent(char* _eventID, void* _callingObject, EventFunction _function)
{
	unsigned int hash = (unsigned int)std::hash<std::string>()(_eventID);

	EventIter iter = m_RegisteredEvents.find(hash);

	if (iter == m_RegisteredEvents.end())
		return false;

	size_t count = iter->second.size();
	for (size_t i = 0; i < count; ++i) {
		if (iter->second[i].CallingObject == _callingObject) {
			EventDelegate temp = iter->second[i];
			iter->second[i] = iter->second[count - 1];
			iter->second[count - 1] = temp;
			iter->second.erase(--iter->second.end());
			return true;
		}
	}

	return false;
}

// === UnregisterFromEvent
//  IN : unsigned int - The hash key of the Event
//       EventFunction - The delegate to unregister
//  OUT : bool - Whether or not the delegate was found and unregistered
//
//  Unregisters the given delegate from a specific event. Returns true if it was found and removed, false otherwise.
bool EventSystem::UnregisterFromEvent(unsigned int _hash, void* _callingObject, EventFunction _function)
{
	EventIter iter = m_RegisteredEvents.find(_hash);

	if (iter == m_RegisteredEvents.end())
		return false;

	size_t count = iter->second.size();
	for (size_t i = 0; i < count; ++i) {
		if (iter->second[i].CallingObject == _callingObject) {
			EventDelegate temp = iter->second[i];
			iter->second[i] = iter->second[count - 1];
			iter->second[count - 1] = temp;
			iter->second.erase(--iter->second.end());
			return true;
		}
	}

	return false;
}

// === QueueEvent
//  IN : char* - Event ID
//       void* - Pointer to the Data that needs to be sent with the Event. Note, this should be an actual pointer to dynamic memory.
//               It will be cleaned up by the event system once it is used.
//  OUT : void
//
//  Queues up an Event to be sent to any listeners it has. The Data that is passed in will need to be a pointer to actual dynamic
//  memory, as the event will not be sent off right away. That memory how ever will be cleaned up by the EventSystem once it uses it.
void EventSystem::QueueEvent(char* _eventID, void* _pData)
{
	EventData data;
	data.ID = (unsigned int)std::hash<std::string>()(_eventID);
	data.pData = _pData;
	m_QueuedEvents.push(data);
}

// === QueueEvent
//  IN : unsigned int - Event ID hash key
//       void* - Pointer to the Data that needs to be sent with the Event. Note, this should be an actual pointer to dynamic memory.
//               It will be cleaned up by the event system once it is used.
//  OUT : void
//
//  Queues up an Event to be sent to any listeners it has. The Data that is passed in will need to be a pointer to actual dynamic
//  memory, as the event will not be sent off right away. That memory how ever will be cleaned up by the EventSystem once it uses it.
void EventSystem::QueueEvent(unsigned int _eventID, void* _pData)
{
	EventData data;
	data.ID = _eventID;
	data.pData = _pData;
	m_QueuedEvents.push(data);
}

// === SendEventNow
//  IN : char* - The Event ID.
//       void* - Pointer to the data that needs to be sent with the event. This memory, if dynamic, will not be cleaned up by the
//               EventSystem.
//  OUT : void
//
//  Sends an Event to all of its' listeners right away. Does not clean up the memory of the data. 
void EventSystem::SendEventNow(char* _eventID, void* _pData)
{
	unsigned int hash = (unsigned int)std::hash<std::string>()(_eventID);

	EventIter iter = m_RegisteredEvents.find(hash);
	if (iter != m_RegisteredEvents.end()) {
		for (size_t i = 0; i < iter->second.size(); ++i) {
			iter->second[i].Delegate(_pData);
		}
	}
}

// === SendEventNow
//  IN : unsigned int - The Event ID hash key.
//       void* - Pointer to the data that needs to be sent with the event. This memory, if dynamic, will not be cleaned up by the
//               EventSystem.
//  OUT : void
//
//  Sends an Event to all of its' listeners right away. Does not clean up the memory of the data. 
void EventSystem::SendEventNow(unsigned int _eventID, void* _pData)
{
	EventIter iter = m_RegisteredEvents.find(_eventID);
	if (iter != m_RegisteredEvents.end()) {
		size_t count = iter->second.size();
		for (size_t i = 0; i < count; ++i) {
			iter->second[i].Delegate(_pData);
		}
	}
}
// ===================== //

// === Static Interface === //

// === GetInstance
//  IN : void
//  OUT : EventSystem* - Pointer to the static Instance of the EventSystem
//
//  Returns the static Instance of the EventSystem. If the EventSystem hasn't been created yet, it creates it, then returns it.
EventSystem* EventSystem::GetInstance()
{
	if (s_pInstance == nullptr) {
		s_pInstance = new EventSystem();
	}

	return s_pInstance;
}

// === DeleteInstance
// IN : void
// OUT : void
//
// Deletes the current instance
void EventSystem::DeleteInstance()
{
	if (s_pInstance != nullptr)
	{
		delete s_pInstance;
		s_pInstance = nullptr;
	}
}
// ======================== //