#include "FunctionRegistry.h"

#include "AssertHelper.h"

#include <hash_map>

// ===== Constructor / Destructor ===== //
FunctionRegistry::FunctionRegistry()
{

}

FunctionRegistry::~FunctionRegistry()
{

}
// ==================================== //

// ===== Interface ===== //

// === RegisterFunction
//  IN : string - Identifier to look up the given function pointer with.
//       Func (Function Pointer) - The Function Pointer to register. Format of void (*Func)(void)
//  OUT : unsigned int - The created hash from the Identifier. (Optional to store and use).
//
//  Registers a given function pointer to an identifier, allowing look ups in the future.
unsigned int FunctionRegistry::RegisterFunction(std::string _id, Func _function)
{
	unsigned int hash = (unsigned int)std::hash<std::string>()(_id);
	funcIter iter = m_RegisteredFunctions.find(hash);

	ASSERT(iter != m_RegisteredFunctions.end(), "Two registered functions using the same Identifier");

	m_RegisteredFunctions.insert(std::make_pair(hash, _function));

	return hash;
}

// === GetFunction
//  IN : string - Identifier of the desired function.
//       Func& - (Out Value) Func Pointer Reference to store the desired function, if found.
//  OUT : bool - Whether or not it found a function linked the given identifier.
//
//  Looks in the map for a function linked to an identifier, stores it in the out parameter. Returns a bool representing if it 
//  found the function or not.
bool FunctionRegistry::CallFunction(std::string _id)
{
	unsigned int hash = (unsigned int)std::hash<std::string>()(_id);

	funcIter iter = m_RegisteredFunctions.find(hash);

	if (iter == m_RegisteredFunctions.end()) {
		return false;
	}

	iter->second();
	return true;
}

// === GetFunction
//  IN : unsigned int - The hash key of the desired function.
//       Func& - (Out Value) Func Pointer Reference to store the desired function, if found.
//  OUT : bool - Whether or not it found a function linked the given identifier.
//
//  Looks in the map for a function linked to an identifier, stores it in the out parameter. Returns a bool representing if it 
//  found the function or not.
bool FunctionRegistry::CallFunction(unsigned int _hash)
{
	funcIter iter = m_RegisteredFunctions.find(_hash);

	if (iter == m_RegisteredFunctions.end()) {
		return false;
	}

	iter->second();
	return true;
}

// ===================== //