#include "Heap.h"

#include <fstream>
using namespace std;


#define HEADER_SIZE		sizeof(Header)
#define FOOTER_SIZE		sizeof(Footer)

#define HEAP_LOCK	if(m_synchronize)EnterCriticalSection(&m_criticalSection)
#define HEAP_UNLOCK	if(m_synchronize)LeaveCriticalSection(&m_criticalSection)

#define HIGH_BIT_32		(1 << 31)


Heap::~Heap()
{
	Destroy();
}




// IN: _size - The size to look for
//
// OUT: Header* - The header to the block of data that was found
//
// Description: Searches the pool for a space that's large enough for the specified size, then returns a Header to that location
Header* Heap::FindBlock(unsigned int _size)
{
	if (m_freeListEntry == nullptr)
		return nullptr;

	Header* temp = m_freeListEntry;

	do
	{
		if (temp->size >= _size)
			return temp;

		temp = temp->next;
	} while (temp != m_freeListEntry);
	return nullptr;
}




// IN: _size - The size to initialize the pool to
//     _sync - A flag for syncronization
//
// OUT: VOID
//
// Description: Initialize the heap to the specified size and set the list members
void Heap::Initialize(unsigned int _size, unsigned int _sync)
{
	m_synchronize = _sync;
	InitializeCriticalSection(&m_criticalSection);

	HEAP_LOCK;

	m_poolSizeTotal = _size;

	m_pool = (char*)malloc(_size);
	// ASSERT(m_pool != nullptr, "Couldn't initialize heap");

	m_firstHeader = (Header*)(m_pool);
	m_freeListEntry = m_firstHeader;
	m_firstHeader->next = m_firstHeader->prev = m_firstHeader;

	m_firstHeader->size = _size - FOOTER_SIZE - HEADER_SIZE;

	m_lastFooter = reinterpret_cast<Footer*>(reinterpret_cast<char*>(m_firstHeader) + HEADER_SIZE + m_firstHeader->size);
	m_lastFooter->size = m_firstHeader->size;

	HEAP_UNLOCK;
}




bool Heap::AllocateSplit(Header* _head, void** _data, int _size)
{
	Footer* freeFoot = nullptr;
	Header* usedHead = nullptr;
	Footer* usedFoot = reinterpret_cast<Footer*>(reinterpret_cast<char*>(_head) + HEADER_SIZE + _head->size);
	bool    doSplit  = false;

	if (_head->size > _size + HEADER_SIZE + FOOTER_SIZE)
	{
		usedHead = reinterpret_cast<Header*>(reinterpret_cast<char*>(usedFoot) - _size - HEADER_SIZE);
		usedFoot->size = _size | HIGH_BIT_32;
		usedHead->size = usedFoot->size;

		freeFoot = reinterpret_cast<Footer*>(reinterpret_cast<char*>(usedHead) - FOOTER_SIZE);
		freeFoot->size = _head->size - HEADER_SIZE - FOOTER_SIZE - _size;
		_head->size = freeFoot->size;

		(*_data) = static_cast<void*>(usedHead + 1);
		doSplit = true;
	}
	return doSplit;
}




void Heap::AllocateNoSplit(Header* _head, void** _data)
{
	Footer* usedFoot = reinterpret_cast<Footer*>(reinterpret_cast<char*>(_head) + HEADER_SIZE + _head->size);

	_head->size |= HIGH_BIT_32;
	usedFoot->size = _head->size;

	m_freeListEntry = _head->next;
	if (_head->next == _head)
	{
		m_freeListEntry = nullptr;
	}
	else
	{
		_head->next->prev = _head->prev;
		_head->prev->next = _head->next;
	}

	(*_data) = static_cast<void*>(_head + 1);
}




bool Heap::DeallocateRight(void* _data)
{
	bool merged = false;
	Header* head = reinterpret_cast<Header*>(reinterpret_cast<char*>(_data) - HEADER_SIZE);
	head->size &= ~HIGH_BIT_32;
	Footer* foot = reinterpret_cast<Footer*>(reinterpret_cast<char*>(head) + head->size + HEADER_SIZE);
	foot->size &= ~HIGH_BIT_32;

	if (foot != m_lastFooter)
	{
		Header* rightHeader = reinterpret_cast<Header*>(reinterpret_cast<char*>(foot)+FOOTER_SIZE);
		if ((rightHeader->size & HIGH_BIT_32) == 0)
		{
			merged = true;
			foot = reinterpret_cast<Footer*>(reinterpret_cast<char*>(rightHeader) + rightHeader->size + HEADER_SIZE);
			foot->size += head->size + HEADER_SIZE + FOOTER_SIZE;
			head->size = foot->size;

			m_freeListEntry = rightHeader->next;
			if (m_freeListEntry->next == m_freeListEntry)
			{
				m_freeListEntry = nullptr;
			}
			else
			{
				rightHeader->next->prev = rightHeader->prev;
				rightHeader->prev->next = rightHeader->next;
			}
		}
	}
	return merged;
}




bool Heap::DeallocateLeft(void* _data)
{
	bool merged = false;
	Header* head = reinterpret_cast<Header*>(reinterpret_cast<char*>(_data) - HEADER_SIZE);
	head->size &= ~HIGH_BIT_32;
	Footer* foot = reinterpret_cast<Footer*>(reinterpret_cast<char*>(head) + head->size + HEADER_SIZE);
	foot->size &= ~HIGH_BIT_32;

	if (head != m_firstHeader)
	{
		Footer* leftFooter = reinterpret_cast<Footer*>(reinterpret_cast<char*>(head)-FOOTER_SIZE);
		if ((leftFooter->size & HIGH_BIT_32) == 0)
		{
			Header* leftHeader = reinterpret_cast<Header*>(reinterpret_cast<char*>(leftFooter)-leftFooter->size - HEADER_SIZE);
			leftHeader->size += foot->size + HEADER_SIZE + FOOTER_SIZE;
			foot->size = leftHeader->size;
			merged = true;
		}
	}
	return merged;
}




void Heap::DeallocateMiddle(void* _data)
{
	bool merged = false;
	Header* head = reinterpret_cast<Header*>(reinterpret_cast<char*>(_data) - HEADER_SIZE);
	head->size &= ~HIGH_BIT_32;
	Footer* foot = reinterpret_cast<Footer*>(reinterpret_cast<char*>(head)+head->size + HEADER_SIZE);
	foot->size &= ~HIGH_BIT_32;

	if (m_freeListEntry != nullptr)
	{
		m_freeListEntry->next->prev = head;
		head->next = m_freeListEntry->next;
		head->prev = m_freeListEntry;
		m_freeListEntry->next = head;
	}
	else
	{
		m_freeListEntry = head;
		m_freeListEntry->next = m_freeListEntry;
		m_freeListEntry->prev = m_freeListEntry;
	}
}




// IN:  _size = The size to allocate
//
// OUT: void* - A pointer to the data allocated
//
// Description: Allocates memory in the pool then returns a pointer to that memory
void* Heap::Allocate(unsigned int _size)
{
	HEAP_LOCK;

	Header *freeHead = FindBlock(_size);
	Footer *freeFoot = nullptr;
	Header *usedHead = nullptr;
	Footer *usedFoot = nullptr;
	void   *data     = nullptr;

	// ASSERT(freeHead != nullptr, "A free chunk of memory was not found. Increase pool size where you're calling MemoryManager::Initialize()");

	if (AllocateSplit(freeHead, &data, _size) == false)
	{
		AllocateNoSplit(freeHead, &data);
	}

	HEAP_UNLOCK;

	return data;
}




// IN: _data - The data to deallocate
//
// OUT: VOID
//
// Description: Gives the specified memory back to the heap
void Heap::Deallocate(void * _data)
{
	HEAP_LOCK;

	bool mergedRight = DeallocateRight(_data);
	bool mergedLeft = DeallocateLeft(_data);

	if (mergedLeft == false)
	{
		DeallocateMiddle(_data);
	}

	HEAP_UNLOCK;
}




// IN: VOID
//
// OUT: VOID
//
// Description: Destroys this heap
void Heap::Destroy()
{
	HEAP_LOCK;

	if (m_pool != nullptr)
		free(m_pool);
	// m_pool = nullptr;

	HEAP_UNLOCK;

	DeleteCriticalSection(&m_criticalSection);
}



