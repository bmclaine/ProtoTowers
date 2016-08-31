///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: RepeatBehavior inherits from DecoratorBehavior.
//              Repeats its child behavior until it's repeated to the specified number.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "../Public/AI/BehaviorTrees/Decorator/RepeatBehavior.h"

RepeatBehavior::RepeatBehavior(int _targetCount, BaseBehavior* _child, std::string& _name)
	: DecoratorBehavior(_child, _name)
	, m_iTarget(_targetCount)
	, m_iCounter(0)
{ }

RepeatBehavior::~RepeatBehavior()
{ }

void RepeatBehavior::Destroy()
{
	m_pChild->Destroy();
	delete m_pChild;
	m_pChild = nullptr;
}

Status RepeatBehavior::Update()
{
	Status status = m_pChild->Tick();
	
	if (status != Status::BT_RUNNING)
		++m_iCounter;
	if (m_iCounter == m_iTarget)
		return Status::BT_SUCCESS;
	else
		return Status::BT_RUNNING;
}

