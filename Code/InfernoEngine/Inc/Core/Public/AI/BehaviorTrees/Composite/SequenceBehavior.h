///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: SequenceBehavior inherits from BaseBehavior.
//              Continues to process children until one returns failure.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

// Engine Includes
#include "../Public/AI/BehaviorTrees/Composite/CompositeBehavior.h"

class SequenceBehavior : public CompositeBehavior
{
private:
	Behaviors::iterator m_itCurrentChild; // -> The current processing child of the sequence

public:
	// Defaults
	SequenceBehavior(std::string& _name = std::string());
	virtual ~SequenceBehavior();
	void Destroy( void ) override;

	// Interface
	virtual void Initialize( void ) override;
	virtual Status Update( void ) override;

	// Accessors
	BehaviorType GetType(void) const override;
};