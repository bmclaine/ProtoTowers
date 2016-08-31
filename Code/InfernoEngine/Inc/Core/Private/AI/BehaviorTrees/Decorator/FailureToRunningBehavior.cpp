///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author:      Doug Berg
//
// Description: If its child returns FAILURE, it returns RUNNING.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "../Public/AI/BehaviorTrees/Decorator/FailureToRunningBehavior.h"

FailureToRunningBehavior::FailureToRunningBehavior(BaseBehavior* _child, std::string& _name)
	: DecoratorBehavior(_child, _name)
{ }

FailureToRunningBehavior::~FailureToRunningBehavior()
{ }

void FailureToRunningBehavior::Destroy()
{ }

Status FailureToRunningBehavior::Update()
{
	Status status = m_pChild->Tick();
	if (status == Status::BT_FAILURE)
		return Status::BT_RUNNING;
	return status;
}