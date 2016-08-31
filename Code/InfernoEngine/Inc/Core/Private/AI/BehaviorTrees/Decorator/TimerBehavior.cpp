///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: TimerBehavior is a child of DecoratorBehavior.
//              Returns RUNNING while the timer is not at its target time.
//              Once at its target time, it calls its childs Tick().
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "../Public/AI/BehaviorTrees/Decorator/TimerBehavior.h"
#include "InfernoTime.h"

TimerBehavior::TimerBehavior(float _time, bool _resetAfterFinish, BaseBehavior* _child, std::string& _name)
	: DecoratorBehavior(_child, _name)
	, m_fTargetTime(_time)
	, m_fElapsedTime(0.0f)
	, m_bReset(_resetAfterFinish)
{ }

TimerBehavior::~TimerBehavior()
{ }

void TimerBehavior::Destroy()
{
	if (m_pChild != nullptr)
	{
		m_pChild->Destroy();
		delete m_pChild;
	}
	m_pChild = nullptr;
}

Status TimerBehavior::Update()
{
	m_fElapsedTime += Time::DeltaTime;
	if (m_fElapsedTime >= m_fTargetTime)
		return m_pChild->Tick();
	else
		return Status::BT_RUNNING;
}

void TimerBehavior::Initialize()
{
	BaseBehavior::Initialize();
	if (m_bReset)
		m_fElapsedTime = 0.0f;
}

