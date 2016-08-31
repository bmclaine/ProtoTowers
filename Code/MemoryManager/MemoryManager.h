///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author:     Doug Berg
// 
//
// Decription: This singleton class is used to manage a defined amount of heaps, depending
//             on how many threads the game is running on.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once


// STL Includes
#include <concrt.h>


// Engine Includes
#include "Heap.h"
#include "HeapLog.h"


#ifdef MEMORYMANAGER_EXPORTS
#define MEMORY_API __declspec(dllexport)
#else
#define MEMORY_API __declspec(dllimport)
#endif

#define KILOBYTE(x)		((unsigned int)1024 * x)
#define MEGABYTE(x)		((unsigned int)1024 * KILOBYTE(x))
#define GIGABYTE(x)		((unsigned int)1024 * MEGABYTE(x))

#define _ATL_DISABLE_NOTHROW_NEW
#define USE_MEMORYMANAGER	1

#define MAX_HEAPS			5


struct HeapData
{
	Heap    heap;           // -> The haep itself
	char    heapName[ 512 ];// -> The name of the heap - helps with debugging
	int     threadID;       // -> The thread ID assigned to this heap when it's created
	HeapLog log;            // -> The heap log which logs data from this heap
};

class MEMORY_API MemoryManager
{
private:
	HeapData         *m_heaps;          // -> An array of heaps
	unsigned int      m_poolSizeTotal;  // -> The total size of the allocation(all heaps combined)
	unsigned int      m_synchronize;    // -> A flag used to protect the heaps from data corruption
	CRITICAL_SECTION  m_criticalSection;// -> A critical section to protect the heap from multithreaded nonsense

	// Defaults are private to protect them from being called
	MemoryManager( void );
	MemoryManager( const MemoryManager& _copy );
	MemoryManager& operator = ( const MemoryManager& _assign );

public:
	// Singleton
	static MemoryManager* GetInstance( void );
	~MemoryManager( void );



	// IN: _size  - The size of memory to create
	//     _file  - The file associated with the memory(i.e. "main.cpp")
	//     _line  - The line that the data was created on
	//     _heaID - Rarely used. Pretty much only used for error checking
	//
	// OUT: void* - The pointer to the newly created data
	//
	// Description: This function is only used by the overloaded "new" calls in OverloadNew.h
	//              ########## DO NOT CALL THIS FUNCTION ##########
	void* Allocate(unsigned int _size, const char* _file = "", unsigned int _line = 0, unsigned int _heapID = 0);




	// IN: _data - The data to give back to the heap
	//     _heapID - Rarely used. Pretty much only used for error checking
	//
	// OUT: VOID
	//
	// Description: This function is only used by the overloaded "delete" calls in OverloadNew.h
	//              ########## DO NOT CALL THIS FUNCTION ##########
	void  Deallocate(void* _data, unsigned int _heapID = 0);




	// IN: _heapSize - The size to make the heap
	//     _heapName - The name of the heap - helps with debugging and leak logging
	//
	// OUT: unsigned int - How many heaps there are after this creaton
	//
	// Description: This function is called by a unique thread, to create memory for said thread.
	unsigned int CreateHeap(unsigned int _heapSize = MEGABYTE(32), const char* _heapName = "UNNAMED_HEAP");




	// IN: VOID
	//
	// OUT: VOID
	//
	// Description: This function destroys the heap that's associated with the thread that's calling this function
	void DestroyHeap( void );
};


