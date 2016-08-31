///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: ActionBehavior derives from BaseBehavior.
//              Is the base class for all action based behaviors.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "../Public/AI/BehaviorTrees/Action/ActionBehavior.h"


ActionBehavior::ActionBehavior(std::string& _name)
	: BaseBehavior(_name)
{ }

ActionBehavior::~ActionBehavior()
{ }

// Interface
void ActionBehavior::Initialize()
{ }

BehaviorType ActionBehavior::GetType() const
{
	return BehaviorType::ACTION;
}

