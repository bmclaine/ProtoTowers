///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: DecoratorBehavior derives from BaseBehavior.
//              A base class that only has one child behavior.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../Public/AI/BehaviorTrees/BaseBehavior.h"

class DecoratorBehavior : public virtual BaseBehavior
{
protected:
	BaseBehavior* m_pChild; // The only child/behavior of the class

public:
	// Defaults
	DecoratorBehavior(BaseBehavior* _child = nullptr, std::string& _name = std::string());
	virtual ~DecoratorBehavior( void );
	void Destroy( void ) override;

	// Accessors
	BehaviorType GetType( void ) const override;
};

