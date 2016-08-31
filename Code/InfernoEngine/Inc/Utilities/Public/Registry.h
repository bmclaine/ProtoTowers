// Author : Jack Gillette
//
// Templated class that allows the specified type of objects to register / unregister themselves into a vector. This uses a vector
// so that it can easily and fastly iterate through all registered objects during runtime. When it Registers a new object, it adds
// it to an available spot, but if there are none, then it does a normal push_back on the vector. When it unregisters an object, it
// swaps the index of the object to unregister and the last valid index, then sets the last valid index to nullptr. This Registry is
// meant to be inheritted by any Modules that keep track of specific objects that can be added or removed during runtime. 
#pragma once

#include <vector>

template<typename type>
class Registry
{
protected:
	// === Shared Variables
	std::vector<type*> m_vRegisteredObjects;

public:
	// === Constructor / Destructor === //
	Registry(unsigned int defaultSize);
	~Registry();
	// ================================ //

	// === Interface === //

	// === RegisterObject
	//  IN : type* - The Pointer to the object to register.
	//  OUT : void
	//
	//  Registers the given object by adding it to the first available index. If there are no available indexes, it does a push_back on
	//  the vector.
	void RegisterObject(type* _object);
	// === UnregisterObject
	//  IN : type* - The Pointer to the object to unregister
	//  OUT : bool - If the object was successfully unregistered
	//
	//  Unregisters the specified Object.
	bool UnregisterObject(type* _object);
	// ================= //

	// === Accessors === //

	// === GetRegisteredObjectCount
	//  IN : void
	//  OUT : unsigned int - Current number of Registered Objects.
	//
	//  Returns the current number of Registered Objects.
	unsigned int GetRegisteredObjectCount();
	// ================= //

	// === GetRegisteredObjects
	//  IN : void
	//  OUT : const std::vector<type*>& - returns a constant list of registered objects
	//
	// Returns the current list of Registered Objects
	const std::vector<type*>& GetRegisteredObjects();
	// ================= //

	// Needed so GraphcisManager can make the registry ordered for UI
	friend class GraphicsManager;
};

// ===== Constructor / Destructor ===== //
template<typename type>
Registry<type>::Registry(unsigned int defaultSize)
{
	m_vRegisteredObjects.reserve(defaultSize);
}

template<typename type>
Registry<type>::~Registry()
{

}
// ==================================== //

// ===== Interface ===== //

// === RegisterObject
//  IN : type* - The Pointer to the object to register.
//  OUT : void
//
//  Registers the given object by adding it to the first available index. If there are no available indexes, it does a push_back on
//  the vector.
template<typename type>
void Registry<type>::RegisterObject(type* _object)
{
	m_vRegisteredObjects.push_back(_object);
}

// === UnregisterObject
//  IN : type* - The Pointer to the object to unregister
//  OUT : bool - If the object was successfully unregistered
//
//  Unregisters the specified Object.
template<typename type>
bool Registry<type>::UnregisterObject(type* _object)
{
	for (size_t i = 0; i < m_vRegisteredObjects.size(); ++i) {
		if (m_vRegisteredObjects[i] == _object) {
			m_vRegisteredObjects[i] = m_vRegisteredObjects[m_vRegisteredObjects.size() - 1];
			m_vRegisteredObjects.erase(--m_vRegisteredObjects.end());
			return true;
		}
	}
	
	return false;
}
// ===================== //

// ===== Accessors ===== //

// === GetRegisteredObjectCount
//  IN : void
//  OUT : unsigned int - Current number of Registered Objects.
//
//  Returns the current number of Registered Objects.
template<typename type>
unsigned int Registry<type>::GetRegisteredObjectCount()
{
	return m_uiCount;
}
// =================== //

// === GetRegisteredObjects
//  IN : void
//  OUT : const std::vector<type*>& - returns a constant list of registered objects
//
// Returns the current list of Registered Objects
template<typename type>
const std::vector<type*>& Registry<type>::GetRegisteredObjects()
{
	return m_vRegisteredObjects;
}
// ================= //