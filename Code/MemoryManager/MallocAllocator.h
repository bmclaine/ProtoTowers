///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: Contains all of the Malloc overrides for custom memory manager
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "MemoryManager.h"
#include <memory>





template<class _Ty>
class MallocAllocator : public std::_Allocator_base<_Ty>
{
public:
	typedef std::_Allocator_base<_Ty> _Mybase;
	typedef typename _Mybase::value_type value_type;
	typedef value_type* pointer;
	typedef value_type& reference;
	typedef const value_type* const_pointer;
	typedef const value_type& const_reference;

	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

	template<class _Other>
	struct rebind
	{
		typedef MallocAllocator<_Other> other;
	};

	pointer address(reference _Val) const
	{
		return (&_Val);
	}

	const_pointer address(const_reference _Val) const
	{
		return (&_Val);
	}

	MallocAllocator() _THROW0() { }

	MallocAllocator(const MallocAllocator<_Ty>&) _THROW0() { }

	template<class _Other>
	MallocAllocator(const MallocAllocator<_Other>&) _THROW0() { }

	template<class _Other>
	MallocAllocator<_Ty>& operator=(const MallocAllocator<_Other>&)
	{
		return (*this);
	}

	void deallocate(pointer _Ptr, size_type)
	{
#if USE_MEMORYMANAGER
		//EDMemoryManager::MemoryManager::GetInstance()->DeAllocate(_Ptr);
		free(_Ptr);
#else
		//::operator delete(_Ptr);
		free(_Ptr);
#endif
	}

	pointer allocate(size_type _Count)
	{
#if USE_MEMORYMANAGER
		//return (_Ty *)EDMemoryManager::MemoryManager::GetInstance()->Allocate(_Count*sizeof(_Ty),false,0,__FILE__,__LINE__);
		return (_Ty *)malloc(_Count*sizeof(_Ty);
#else
		//return (_Allocate(_Count, (pointer)0));
		return (_Ty *)malloc(_Count*sizeof(_Ty));
#endif
	}

	pointer allocate(size_type _Count, const void *)
	{
#if USE_MEMORY_MANAGER
		//return (_Ty *)EDMemoryManager::MemoryManager::GetInstance()->Allocate(_Count*sizeof(_Ty),false,0,__FILE__,__LINE__);
		return (_Ty *)malloc(_Count*sizeof(_Ty);
#else
		//return (allocate(_Count));
		return (_Ty *)malloc(_Count*sizeof(_Ty));
#endif
	}

	void construct(pointer _Ptr, const _Ty& _Val)
	{
		std::_Construct(_Ptr, _Val);
	}

	void destroy(pointer _Ptr)
	{
		std::_Destroy(_Ptr);
	}

	size_t max_size() const _THROW0()
	{
		size_t _Count = (size_t)(-1) / sizeof(_Ty);
		return (0 < _Count ? _Count : 1);
	}
};





template<class _Ty, class _Other> inline
bool operator==(const MallocAllocator<_Ty>&, const MallocAllocator<_Other>&) _THROW0()
{
	return (true);
}

template<class _Ty, class _Other> inline
bool operator!=(const MallocAllocator<_Ty>&, const MallocAllocator<_Other>&) _THROW0()
{
	return (false);
}




// CLASS MallocAllocator<void>
template<> class _CRTIMP2_PURE MallocAllocator<void>
{
public:
	typedef void _Ty;
	typedef _Ty *pointer;
	typedef const _Ty *const_pointer;
	typedef _Ty value_type;

	template<class _Other>
	struct rebind
	{
		typedef MallocAllocator<_Other> other;
	};

	MallocAllocator() _THROW0() { }

	MallocAllocator(const MallocAllocator<_Ty>&) _THROW0() { }

	template<class _Other>
	MallocAllocator(const MallocAllocator<_Other>&) _THROW0() { }

	template<class _Other>
	MallocAllocator<_Ty>& operator=(const MallocAllocator<_Other>&)
	{
		return (*this);
	}
};

