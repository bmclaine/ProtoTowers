// Author : Jack Gillette
//
// Preallocated pool of a specific object, that can be expanded during runtime if needed.
#pragma once

#include "AssertHelper.h"

#include <vector>

template<typename type>
class AllocationPool
{
private:
	// === Variables
	std::vector<type*> m_vObjectPool;
	type* m_pOriginalObject;
	unsigned int m_uiPoolIncreaseSize;
	unsigned int m_uiAllocatedObjects;
	bool m_bCleanupMemory;

public:
	// === Constructor / Destructor === //
	AllocationPool(type* _object, unsigned int _amount, bool _cleanupMemory = true);
	AllocationPool();
	~AllocationPool();
	// ================================ //

	// === Interface === //
	type* Allocate();
	void Deallocate(type* _object);
	void CleanupMemory();
	// ================= //
};

// ===== Constructor / Destructor ===== //
template<typename type>
AllocationPool<type>::AllocationPool(type* _object, unsigned int _amount, bool _cleanupMemory)
{
	m_uiAllocatedObjects = 0;
	m_uiPoolIncreaseSize = 10;
	m_pOriginalObject = _object;
	m_bCleanupMemory = _cleanupMemory;

	m_vObjectPool.resize(_amount);
	for (unsigned int i = 0; i < _amount; ++i) {
		m_vObjectPool[i] = new type(*m_pOriginalObject);
		m_vObjectPool[i]->Init();
		m_vObjectPool[i]->SetActive(false);
	}
}

template<typename type>
AllocationPool<type>::AllocationPool()
{
	m_uiAllocatedObjects = 0;
	m_uiPoolIncreaseSize = 10;
	m_pOriginalObject = nullptr;
	m_bCleanupMemory = true;
}

template<typename type>
AllocationPool<type>::~AllocationPool()
{
	if (m_bCleanupMemory) {
		size_t count = m_vObjectPool.size();
		for (size_t i = 0; i < count; ++i) {
			delete m_vObjectPool[i];
		}
	}
}
// ==================================== //

// ===== Interface ===== //
template<typename type>
type* AllocationPool<type>::Allocate()
{
	// === Do we any available objects left?
	if (m_uiAllocatedObjects == m_vObjectPool.size()) {
		if (m_uiPoolIncreaseSize == 0)
			return nullptr;

		m_vObjectPool.resize(m_vObjectPool.size() + m_uiPoolIncreaseSize);
		size_t count = m_vObjectPool.size();
		for (size_t i = m_uiAllocatedObjects; i < count; ++i) {
			m_vObjectPool[i] = new type(*m_pOriginalObject);
			m_vObjectPool[i]->Init();
			m_vObjectPool[i]->SetActive(false);
		}
	}

	return m_vObjectPool[m_uiAllocatedObjects++];
}

template<typename type>
void AllocationPool<type>::Deallocate(type* _object)
{
	if (m_uiAllocatedObjects == 0) {
		// === Must be the Original Object trying to deallocate
		return;
	}

	// === Move the Deallocated Object to the end of the vector
	unsigned int i;
	for (i = 0; i < m_uiAllocatedObjects; ++i) {
		if (m_vObjectPool[i] == _object) {
			type* tempPtr = m_vObjectPool[i];
			m_vObjectPool[i] = m_vObjectPool[m_uiAllocatedObjects - 1];
			m_vObjectPool[m_uiAllocatedObjects - 1] = tempPtr;
			break;
		}
	}

	if (!ASSERT(i <= m_uiAllocatedObjects, "Tried to Deallocate an object that wasn't allocated.")) {
		return;
	}
	
	// === Reset the Deallocated Object
	((GameObject*)(_object))->SetActive(false);
	*((GameObject*)(_object)) = *((GameObject*)(m_pOriginalObject));

	--m_uiAllocatedObjects;
}
// ===================== //