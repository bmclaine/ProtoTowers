///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: Will return the initial status from its child until it returns NOT RUNNING.
//              Every frame after, will always return SUCCESS.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "../Public/AI/BehaviorTrees/Decorator/DoOnceBehavior.h"

DoOnceBehavior::DoOnceBehavior(BaseBehavior* _child, std::string& _name)
	: DecoratorBehavior(_child, _name)
	, m_bExecuted(false)
{ }

DoOnceBehavior::~DoOnceBehavior()
{ }

void DoOnceBehavior::Destroy()
{ }

Status DoOnceBehavior::Update()
{
	Status status = Status::BT_INVALID;

	if (m_bExecuted == false)
		status = m_pChild->Tick();
	else
		status = Status::BT_SUCCESS;

	if (status != Status::BT_RUNNING)
		m_bExecuted = true;

	return status;
}

