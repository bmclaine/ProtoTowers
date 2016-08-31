///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: Contains all of the allocator overrides for custom memory manager
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "MemoryManager.h"
#include <memory>





template<class _Ty>
class Allocator
	: public std::_Allocator_base<_Ty>
{
public:
	typedef std::_Allocator_base<_Ty> _Mybase;
	typedef typename _Mybase::value_type value_type;
	typedef value_type* pointer;
	typedef value_type& reference;
	typedef const value_type *const_pointer;
	typedef const value_type & const_reference;

	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

	template<class _Other>
	struct rebind
	{
		typedef Allocator<_Other> other;
	};

	pointer address(reference _Val) const
	{
		return (&_Val);
	}

	const_pointer address(const_reference _Val) const
	{
		return (&_Val);
	}

	Allocator() _THROW0() { }

	Allocator(const Allocator<_Ty>&) _THROW0() { }

	template<class _Other>
	Allocator(const Allocator<_Other>&) _THROW0() { }

	template<class _Other>
	Allocator<_Ty>& operator=(const Allocator<_Other>&)
	{
		return (*this);
	}

	void deallocate(pointer _Ptr, size_type)
	{	
#if USE_MEMORYMANAGER
		EDMemoryManager::MemoryManager::GetInstance()->DeAllocate(_Ptr);
#else
		::operator delete(_Ptr);
#endif
	}

	pointer allocate(size_type _Count)
	{
#if USE_MEMORYMANAGER
		return (_Ty *)EDMemoryManager::MemoryManager::GetInstance()->Allocate(static_cast<unsigned int>(_Count*sizeof(_Ty)), 0, __FILE__, __LINE__);
#else
		return (_Allocate(_Count, (pointer)0));
#endif
	}

	pointer allocate(size_type _Count, const void *)
	{
#if USE_MEMORYMANAGER
		return (_Ty *)EDMemoryManager::MemoryManager::GetInstance()->Allocate(_Count*sizeof(_Ty), 0, __FILE__, __LINE__);
#else
		return (allocate(_Count));
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


template<class _Ty,
class _Other> inline
	bool operator==(const Allocator<_Ty>&, const Allocator<_Other>&) _THROW0()
{
	return (true);
}

template<class _Ty,
class _Other> inline
	bool operator!=(const Allocator<_Ty>&, const Allocator<_Other>&) _THROW0()
{
	return (false);
}





template<> class _CRTIMP2_PURE Allocator<void>
{
public:
	typedef void _Ty;
	typedef _Ty *pointer;
	typedef const _Ty *const_pointer;
	typedef _Ty value_type;

	template<class _Other>
	struct rebind
	{
		typedef Allocator<_Other> other;
	};

	Allocator() _THROW0() { }

	Allocator(const Allocator<_Ty>&) _THROW0() { }

	template<class _Other>
	Allocator(const Allocator<_Other>&) _THROW0() { }

	template<class _Other>
	Allocator<_Ty>& operator=(const Allocator<_Other>&)
	{
		return (*this);
	}
};

