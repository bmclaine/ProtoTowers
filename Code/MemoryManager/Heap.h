///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: The MemoryManager uses this class.
//              This is the actually heap of memory for each thread.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once


// STL Includes
#include <Windows.h>
#include <concrt.h>


// Engine Includes


struct Header
{
	// Using the high bit of 'size' to represent if the block is used or not
	// HIGH BIT of size == 1 - Used block
	// HIGH BIT of size == 0 - Free block

	unsigned int size; // -> The size of the data after this header
	Header*      prev; // -> The previous block of data in the list
	Header*      next; // -> The next block of data in the list
};
struct Footer
{
	// Using the high bit of 'size' to represent if the block is used or not
	// HIGH BIT of size == 1 - Used block
	// HIGH BIT of size == 0 - Free block

	unsigned int size; // -> The size of the data after this header
};



class Heap
{
private:
	friend class MemoryManager;

	char             *m_pool;           // -> The actual pool of memory
	Header           *m_freeListEntry;  // -> The starting point of the list of free memory
	Header           *m_firstHeader;    // -> The first header in the list
	Footer           *m_lastFooter;     // -> The last header in the list
	unsigned int      m_poolSizeTotal;  // -> The size of the entire pool of memory, not including the header and footers

	unsigned int      m_synchronize;    // -> A flag to keep things thread safe
	CRITICAL_SECTION  m_criticalSection;// -> A critical section to prevent threading nonsense

	bool AllocateSplit   ( Header* _head, void** _data, int _size );// -> The entire pool has to split to allocate memory
	void AllocateNoSplit ( Header* _head, void** _data );           // -> The entire pool did not have to split to allocate memory
	bool DeallocateRight ( void*   _data );                         // -> Memory has been removed - This tries to merge it to its right in the pool
	bool DeallocateLeft  ( void*   _data );                         // -> Memory has been removed - This tries to merge it to its left in the pool
	void DeallocateMiddle( void*   _data );                         // -> Memory has been removed - This merges to its left and right in the pool

public:
	// Default
	~Heap( void );
	



	// IN: _size - The size to look for
	//
	// OUT: Header* - The header to the block of data that was found
	//
	// Description: Searches the pool for a space that's large enough for the specified size, then returns a Header to that location
	Header *FindBlock(unsigned int _size);




	// IN: _size - The size to initialize the pool to
	//     _sync - A flag for syncronization
	//
	// OUT: VOID
	//
	// Description: Initialize the heap to the specified size and set the list members
	void Initialize(unsigned int _size, unsigned int _sync = 0);




	// IN:  _size = The size to allocate
	//
	// OUT: void* - A pointer to the data allocated
	//
	// Description: Allocates memory in the pool then returns a pointer to that memory
	void *Allocate( unsigned int _size );




	// IN: _data - The data to deallocate
	//
	// OUT: VOID
	//
	// Description: Gives the specified memory back to the heap
	void Deallocate(void * _data);




	// IN: VOID
	//
	// OUT: VOID
	//
	// Description: Destroys this heap
	void Destroy( void );
};