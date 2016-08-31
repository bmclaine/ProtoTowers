///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: Handles condition statements in the behavior tree. Resembles an 'if' statement.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "AI\BehaviorTrees\BaseBehavior.h"

class ConditionBehavior : public BaseBehavior
{
public:
	ConditionBehavior(std::string& _name = std::string());
	virtual ~ConditionBehavior();
};