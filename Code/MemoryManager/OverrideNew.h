///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: Overridding all of the 'new' and 'delete' operators to use my customr manager.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "MemoryManager.h"


#ifdef new
// #undef new
#endif





inline void* operator new (size_t _size)
{
	return MemoryManager::GetInstance()->Allocate(static_cast<unsigned int>(_size));
}
inline void* operator new[](size_t _size)
{
	return MemoryManager::GetInstance()->Allocate(static_cast<unsigned int>(_size), nullptr, 0);
}
inline void* operator new (size_t _size, char* _file, unsigned int _line)
{
	return MemoryManager::GetInstance()->Allocate(static_cast<unsigned int>(_size), _file, _line);
}
inline void* operator new[](size_t _size, char* _file, unsigned int _line)
{
	return MemoryManager::GetInstance()->Allocate(static_cast<unsigned int>(_size), _file, _line);
}



inline void operator delete  (void* _data)
{
	MemoryManager::GetInstance()->Deallocate((char*)_data);
}
inline void operator delete[](void* _data)
{
	MemoryManager::GetInstance()->Deallocate((char*)_data);
}
inline void operator delete  (void* _data, char* _file, unsigned int _line)
{
	MemoryManager::GetInstance()->Deallocate((char*)_data);
}
inline void operator delete[](void* _data, char* _file, unsigned int _line)
{
	MemoryManager::GetInstance()->Deallocate((char*)_data);
}


// Redefine NEW & DELETE
#if USE_MEMORYMANAGER

#define new    new(__FILE__, __LINE__)
#define delete delete

#endif
