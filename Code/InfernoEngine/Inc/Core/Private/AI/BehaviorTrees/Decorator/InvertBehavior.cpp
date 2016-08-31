///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: InvertBehavior is a child from DecoratorBehavior.
//              Returns the inverse Status of what its child returns. (i.e. Return !Result)
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "../Public/AI/BehaviorTrees/Decorator/InvertBehavior.h"

InvertBehavior::InvertBehavior(BaseBehavior* _child, std::string& _name)
	: DecoratorBehavior(_child, _name)
{ }

InvertBehavior::~InvertBehavior()
{ }

void InvertBehavior::Destroy()
{
	m_pChild->Destroy();
	delete m_pChild;
	m_pChild = nullptr;
}

Status InvertBehavior::Update()
{
	Status status = m_pChild->Tick();

	if (status != Status::BT_RUNNING)
		return (status == Status::BT_SUCCESS) ? Status::BT_FAILURE : Status::BT_SUCCESS;
	else
		return status;
}

