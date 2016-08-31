///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: ActionBehavior derives from BaseBehavior.
//              Is the base class for all action based behaviors.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../Public/AI/BehaviorTrees/BaseBehavior.h"

class ActionBehavior : public virtual BaseBehavior
{
public:
	ActionBehavior( std::string& _name = std::string() );
	virtual ~ActionBehavior( void );

	// Interface
	virtual void Initialize(void) override;

	// Accessors
	BehaviorType GetType( void ) const override;
};

