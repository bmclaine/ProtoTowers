///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: DecoratorBehavior derives from BaseBehavior.
//              A base class that only has one child behavior.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "../Public/AI/BehaviorTrees/Decorator/DecoratorBehavior.h"

DecoratorBehavior::DecoratorBehavior(BaseBehavior* _child, std::string& _name)
	: BaseBehavior(_name)
	, m_pChild(_child)
{ }

DecoratorBehavior::~DecoratorBehavior()
{ }

void DecoratorBehavior::Destroy()
{
	if (m_pChild != nullptr)
	{
		m_pChild->Destroy();
		delete m_pChild;
	}
	m_pChild = nullptr;
}

// Accessors
BehaviorType DecoratorBehavior::GetType() const
{
	return BehaviorType::DECORATOR;
}

