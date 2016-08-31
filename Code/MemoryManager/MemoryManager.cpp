///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author:     Doug Berg
// 
//
// Decription: This singleton class is used to manage a defined amount of heaps, depending
//             on how many threads the game is running on.
//
///////////////////////////////////////////////////////////////////////////////////////////////////


// STL Includes
using namespace std;


// Engine Includes
#include "MemoryManager.h"


#define HEADER_SIZE		sizeof(Header)
#define FOOTER_SIZE		sizeof(Footer)




/*static*/ MemoryManager* MemoryManager::GetInstance()
{
	static MemoryManager s_instance;
	return &s_instance;
}




MemoryManager::MemoryManager()
{
	m_heaps = (HeapData*)malloc(sizeof(HeapData) * MAX_HEAPS);

	for (int i = 0; i < MAX_HEAPS; ++i)
		new (&m_heaps[i].log) HeapLog();
	
	InitializeCriticalSection(&m_criticalSection);
	CreateHeap(GIGABYTE(2), "Main Thread Heap");
}





MemoryManager::~MemoryManager()
{
	for (int i = 0; i < MAX_HEAPS; ++i)
		DestroyHeap();

	m_heaps[0].heap.~Heap();

	if (m_heaps)
		free(m_heaps);

	DeleteCriticalSection(&m_criticalSection);
}




// IN: _size  - The size of memory to create
//     _file  - The file associated with the memory(i.e. "main.cpp")
//     _line  - The line that the data was created on
//     _heaID - Rarely used. Pretty much only used for error checking
//
// OUT: void* - The pointer to the newly created data
//
// Description: This function is only used by the overloaded "new" calls in OverloadNew.h
//              ########## DO NOT CALL THIS FUNCTION ##########
void* MemoryManager::Allocate(unsigned int _size, const char* _file, unsigned int _line, unsigned int _heapID)
{
	if (_heapID)
	{
		if (_heapID < MAX_HEAPS)
		{
			void* data = m_heaps[_heapID].heap.Allocate(_size);
#if _DEBUG
			LogInfo info;
			memcpy(info.file, _file, strlen(_file) + 1);
			info.line = _line;
			info.size = _size;
			m_heaps[_heapID].log.Add(data, info);
#endif
			return data;
		}
		// ASSERT(false, "Heap is not valid");
		return nullptr;
	}


	const unsigned int currentThreadID = GetCurrentThreadId();
	for (unsigned int i = 0; i < MAX_HEAPS; ++i)
	{
		if (currentThreadID == m_heaps[i].threadID)
		{
			void* data = m_heaps[i].heap.Allocate(_size);
			// ASSERT(data != nullptr, "Heap is not valid");

			if (data != nullptr)
			{
#if _DEBUG
				LogInfo info;
				memcpy(info.file, _file, strlen(_file) + 1);
				info.line = _line;
				info.size = _size;
				m_heaps[i].log.Add(data, info);
#endif
				return data;
			}
		}
	}

	// ASSERT(false, "No memory available for this thread");
	return nullptr;
}




// IN: _data - The data to give back to the heap
//     _heapID - Rarely used. Pretty much only used for error checking
//
// OUT: VOID
//
// Description: This function is only used by the overloaded "delete" calls in OverloadNew.h
//              ########## DO NOT CALL THIS FUNCTION ##########
void MemoryManager::Deallocate(void* _data, unsigned int _heapID)
{
	// ASSERT(_data != nullptr, "Tried to deallocate nullptr);
	if (_data == nullptr)
		return;

	if (_heapID)
	{
		if (_heapID < MAX_HEAPS)
		{
#if _DEBUG
			m_heaps[_heapID].log.Remove(_data);
#endif
			m_heaps[_heapID].heap.Deallocate(_data);
			return;
		}
		// ASSERT(false, "Heap not valid");
	}

	int index = -1;
	for (unsigned int i = 0; i < MAX_HEAPS; ++i)
	{
		if (_data > m_heaps[i].heap.m_firstHeader && _data < m_heaps[i].heap.m_lastFooter)
		{
			index = i;
			break;
		}
	}
#if _DEBUG
		m_heaps[index].log.Remove(_data);
#endif
	m_heaps[index].heap.Deallocate(_data);
}




// IN: _heapSize - The size to make the heap
//     _heapName - The name of the heap - helps with debugging and leak logging
//
// OUT: unsigned int - How many heaps there are after this creaton
//
// Description: This function is called by a unique thread, to create memory for said thread.
unsigned int MemoryManager::CreateHeap(unsigned int _heapSize, const char* _heapName)
{
	static unsigned int i = 0;
	unsigned int returnValue = -1;

	// ASSERT( _heapSize > HEADER_SIZE + FOOTER_SIZE,  "Requested memory size is too small. Try allocating (X + (N / 8)) bytes, where X is the number of bytes you're requesting.");
	if (_heapSize < HEADER_SIZE + FOOTER_SIZE)
	{
		int i = 0;
	}

	EnterCriticalSection(&m_criticalSection);

	if (i < MAX_HEAPS)
	{
		m_heaps[i].heap.Initialize(_heapSize, 1);

		if (_heapName != nullptr)
			memcpy(m_heaps[i].heapName, _heapName, strlen(_heapName) + 1);

		m_heaps[i].threadID = GetCurrentThreadId();
		returnValue = i;
		++i;
	}


	LeaveCriticalSection(&m_criticalSection);
	return returnValue;
}




// IN: VOID
//
// OUT: VOID
//
// Description: This function destroys the heap that's associated with the thread that's calling this function
void MemoryManager::DestroyHeap()
{
	EnterCriticalSection(&m_criticalSection);

	int threadID = GetCurrentThreadId();

	for (unsigned int i = 0; i < MAX_HEAPS; ++i)
	{
		if (threadID == m_heaps[i].threadID)
		{
			m_heaps[i].log.WriteLogFile(m_heaps[i].heapName);
			m_heaps[i].log.~HeapLog();
			m_heaps[i].heap.~Heap();
			m_heaps[i].threadID = -1;
			break;
		}
	}

	LeaveCriticalSection(&m_criticalSection);
}





