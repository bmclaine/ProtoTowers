// Author: Jonathan Gyurkovics
//
// A simple templated flag class that will handle flag based operations

#pragma once

#include "AssertHelper.h"

#define INITIAL_POWER_OFFSET 2

// A simple templated flag class to handle flags for systems
template<typename T>
class Flags
{
private:
	T* m_pFlags;							// Stores the pointer to the flags container
	unsigned int m_uiVarCount;			// Stores how many varibales are in the container
	const unsigned int m_uiBitsPerVar;	// Stores how many bits are total in each variable
	const unsigned int m_uiPower;			// Stores the power of 2 to use for bitwise operations

public:
	Flags(unsigned int flagCount);
	~Flags();

	Flags<T>& operator=(const Flags<T>& rhs);

	// IN: unsigned int - flag to get
	// OUT: bool - status of the flag
	//
	// Returns the status of the requested flag
	bool GetFlag(unsigned int flag) const;
	// IN: unsigned int - flag to get
	// OUT: int - status of the flag
	//
	// Returns the status of the requested flag as an int
	int GetFlagAsInt(unsigned int flag) const;
	// IN: flag - flag to set
	//     unsigned int - value to set the flag to
	// OUT: void
	//
	// Sets the requested flag to the requested value
	void SetFlag(unsigned int flag, unsigned int value);
	// IN: void
	// OUT: void
	//
	// Resets all flags to off
	void ClearFlags();

	// IN: unsigned int - number of flags to resize to
	// OUT: void
	//
	// Resizes the size of the stored buffer based on the passed in flags
	void ResizeFlags(unsigned int flagCount);
};

// Default CTor
// Initializes all values
// We store the number of bits - 1 to allow for a bitwise & to do a faster mod operation
// We store the size + 2 to get the proper power of 2 so we can bit shift instead of divide to find which variable we need to use
template<typename T>
Flags<T>::Flags(unsigned int flagCount) : m_uiBitsPerVar((sizeof(T) * 8) - 1), m_uiPower(sizeof(T) + INITIAL_POWER_OFFSET)
{
	m_pFlags = nullptr;
	m_uiVarCount = 0;

	ResizeFlags(flagCount);
}

template<typename T>
Flags<T>::~Flags()
{
	if (m_pFlags != nullptr)
	{
		delete[] m_pFlags;
		m_pFlags = nullptr;
	}
}

template<typename T>
Flags<T>& Flags<T>::operator=(const Flags<T>& rhs)
{
	ResizeFlags(rhs.m_uiVarCount * rhs.m_uiBitsPerVar);

	for (size_t var = 0; var < m_uiVarCount; var++)
		m_pFlags[var] = rhs.m_pFlags[var];

	return *this;
}

// IN: unsigned int - flag to get
// OUT: bool - status of the flag
//
// Returns the status of the requested flag
template<typename T>
bool Flags<T>::GetFlag(unsigned int flag) const
{
	unsigned int variable = flag >> m_uiPower;
	unsigned int bit = flag & m_uiBitsPerVar;

#if !defined(SHIPPING)
	if (ASSERT(variable >= m_uiVarCount, L"Flag out of bounds!"))
#else 
	if (variable >= m_uiVarCount)
#endif
		return false;

	if (m_pFlags[variable] & (1 << bit))
		return true;
	
	return false;
}

// IN: unsigned int - flag to get
// OUT: int - status of the flag
//
// Returns the status of the requested flag as an intd
template<typename T>
int Flags<T>::GetFlagAsInt(unsigned int flag) const
{
	unsigned int variable = flag >> m_uiPower;
	unsigned int bit = flag & m_uiBitsPerVar;

#if !defined(SHIPPING)
	if (ASSERT(variable >= m_uiVarCount, L"Flag out of bounds!"))
#else 
	if (variable >= m_uiVarCount)
#endif
		return 0;

	if (m_pFlags[variable] & (1 << bit))
		return 1;

	return 0;
}

// IN: flag - flag to set. 1 - ON  0 - OFF
//     unsigned int - value to set the flag to
// OUT: void
//
// Sets the requested flag to the requested value
template<typename T>
void Flags<T>::SetFlag(unsigned int flag, unsigned int value)
{
	unsigned int variable = flag >> m_uiPower;
	unsigned int bit = flag & m_uiBitsPerVar;

#if !defined(SHIPPING)
	if (ASSERT(variable >= m_uiVarCount, L"Flag out of bounds!"))
#else 
	if (variable >= m_uiVarCount)
#endif
		return;

	if (value)
		m_pFlags[variable] |= (1 << bit);
	else
		m_pFlags[variable] &= ~(1 << bit);
}

// IN: void
// OUT: void
//
// Resets all flags to off
template<typename T>
void Flags<T>::ClearFlags()
{
	for (unsigned int var = 0; var < m_uiVarCount; var++)
		m_pFlags[var] = 0;
}

// IN: unsigned int - number of flags to resize to
// OUT: void
//
// Resizes the size of the stored buffer based on the passed in flags
template<typename T>
void Flags<T>::ResizeFlags(unsigned int flagCount)
{
	// Get the total number of variables needed to be created based on the bit size of the variable
	unsigned int newVarCount = (flagCount % (sizeof(T) * 8) != 0) ? (flagCount / 8) + 1 : flagCount / 8;

	if (newVarCount == m_uiVarCount)
		return;

	T* temp = new T[newVarCount];

	for (unsigned int var = 0; var < newVarCount; var++)
		temp[var] = 0;

	if (m_pFlags != nullptr)
	{
		// Determine how many times we need to loop and copy over all data that will fit in the new size
		unsigned int maxLoopCounter = (newVarCount > m_uiVarCount) ? m_uiVarCount : newVarCount;
		for (unsigned int var = 0; var < maxLoopCounter; var++)
			temp[var] = m_pFlags[var];

		// Delete our old flags
		delete[] m_pFlags;
	}

	// Store all data in the current holders
	m_pFlags = temp;
	m_uiVarCount = newVarCount;
}