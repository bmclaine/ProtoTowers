// Author : Jack Gillette
//
// Allows classes to register their own functions to a map, that can then be retrieved by an ID. Since it is planned to only be
// used by the UIComponents for UIEvents, it currently only handles functions of : void (*func)(void)
#pragma once

#include <functional>
#include <map>
#include <string>

class FunctionRegistry
{
private:
	// === TypeDefs
	typedef std::function<void()> Func;
	typedef std::map<unsigned int, Func>::iterator funcIter;

	// === Variables
	std::map<unsigned int, Func> m_RegisteredFunctions;

public:
	// === Constructor / Destructor === //
	FunctionRegistry();
	~FunctionRegistry();
	// ================================ //

	// === Interface === //

	// === RegisterFunction
	//  IN : string - Identifier to look up the given function pointer with.
	//       Func (Function Pointer) - The Function Pointer to register. Format of void (*Func)(void)
	//  OUT : unsigned int - The created hash from the Identifier. (Optional to store and use).
	//
	//  Registers a given function pointer to an identifier, allowing look ups in the future.
	unsigned int RegisterFunction(std::string _id, Func _function);
	// === CallFunction
	//  IN : string - Identifier of the desired function.
	//  OUT : bool - Whether or not it found a function linked the given identifier.
	//
	//  Looks in the map for a function linked to an identifier, and calls it if found. Returns a bool representing if it 
	//  found the function or not.
	bool CallFunction(std::string _id);
	// === CallFunction
	//  IN : unsigned int - The hash key of the desired function.
	//  OUT : bool - Whether or not it found a function linked the given identifier.
	//
	//  Looks in the map for a function linked to an identifier, and calls it if found. Returns a bool representing if it 
	//  found the function or not.
	bool CallFunction(unsigned int _hash);
	// ================= //
};