///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: BaseBehavior is the base class of ALL behaviors.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#define REGISTER_TREE_STATES(x) x,
typedef enum
{
#include "StatusTypes.h"
	BT_STATE_MAX
} Status;
#undef REGISTER_TREE_STATES
#define REGISTER_TREE_TYPES(x) x,
typedef enum
{
#include "BehaviorTypes.h"
	BT_TYPES_MAX
} BehaviorType;
#undef REGISTER_TREE_TYPES

#include <string>

class BaseBehavior
{
protected:
	Status      m_eStatus; // -> The status of the behavior(Running, success, failure, etc.)
	std::string m_sName;   // -> The name of the behavior

public:
	// Defaults
	BaseBehavior(std::string& _name = std::string());
	virtual ~BaseBehavior() = 0;
	virtual void Destroy( void ) = 0;

	// Interface
	Status Tick( void );
	virtual void Initialize( void );
	virtual Status Update ( void ) = 0;
	virtual void OnTerminate(Status _status);
	void Abort( void );
	void Reset( void );
	
	// Accessors
	bool IsTerminated( void ) const;
	bool IsRunning( void ) const;
	Status GetStatus( void ) const;
	std::string GetName( void ) const;
	virtual BehaviorType GetType( void ) const;

	// Mutators
	void SetName(std::string _name);
};

