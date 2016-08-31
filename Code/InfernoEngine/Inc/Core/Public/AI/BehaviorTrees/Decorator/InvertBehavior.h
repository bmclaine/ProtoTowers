///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: InvertBehavior is a child from DecoratorBehavior.
//              Returns the inverse Status of what its child returns. (i.e. Return !Result)
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../Public/AI/BehaviorTrees/Decorator/DecoratorBehavior.h"

class InvertBehavior : public virtual DecoratorBehavior
{
public:
	InvertBehavior(BaseBehavior* _child = nullptr, std::string& _name = std::string());
	~InvertBehavior( void );
	void Destroy( void ) override;

	virtual Status Update( void ) override;
};

