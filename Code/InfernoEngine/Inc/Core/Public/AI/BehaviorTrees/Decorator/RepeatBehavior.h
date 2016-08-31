///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: RepeatBehavior inherits from DecoratorBehavior.
//              Repeats its child behavior until it's repeated to the specified number.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../Public/AI/BehaviorTrees/Decorator/DecoratorBehavior.h"

class RepeatBehavior : public virtual DecoratorBehavior
{
private:
	int m_iTarget; // -> The amount of times to repeat the childs behavior
	int m_iCounter;// -> The amount of times the childs behavior has been ticked

public:
	RepeatBehavior( int _targetCount = 0, BaseBehavior* _child = nullptr, std::string& _name = std::string() );
	~RepeatBehavior( void );
	void Destroy( void ) override;

	virtual Status Update( void ) override;
};

