///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author:      Doug Berg
//
// Description: If its child returns FAILURE, it returns RUNNING.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "DecoratorBehavior.h"

class FailureToRunningBehavior : public DecoratorBehavior
{
public:
	FailureToRunningBehavior(BaseBehavior* _child, std::string& _name = std::string());
	~FailureToRunningBehavior();

	void Destroy() override;
	Status Update() override;
};