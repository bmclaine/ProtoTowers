///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: Repeats the child behavior for the specified amount of time.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "../Public/AI/BehaviorTrees/Decorator/RepeatForTime.h"
#include "InfernoTime.h"

RepeatForTime::RepeatForTime(float _time, BaseBehavior* _child, bool _resetOnInit, std::string& _name)
	: DecoratorBehavior(_child, _name)
	, m_fElapsedTime(0.0f)
	, m_fTargetTime(_time)
	, m_bReset(_resetOnInit)
{ }

RepeatForTime::~RepeatForTime()
{ }

void RepeatForTime::Destroy()
{ }

Status RepeatForTime::Update()
{
	m_fElapsedTime += Time::DeltaTime;
	Status status = m_pChild->Tick();

	if (m_fElapsedTime > m_fTargetTime)
		return status;
	return Status::BT_RUNNING;
}

void RepeatForTime::Initialize()
{
	DecoratorBehavior::Initialize();
	if (m_bReset)
		m_fElapsedTime = 0.0f;
}

