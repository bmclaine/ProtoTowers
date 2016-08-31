///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: Will make an entity idle in position.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "AI\BehaviorTrees\Decorator\WaitBehavior.h"
#include "InfernoTime.h"

WaitBehavior::WaitBehavior(float _time, std::string& _name)
	: DecoratorBehavior(nullptr, _name)
	, m_fTargetTime(_time)
	, m_fElapsedTime(0.0f)
{ }

WaitBehavior::~WaitBehavior()
{ }

void WaitBehavior::Initialize()
{
	m_fElapsedTime = 0.0f;
}

Status WaitBehavior::Update()
{
	m_fElapsedTime += Time::DeltaTime;

	if (m_fElapsedTime > m_fTargetTime)
		return Status::BT_SUCCESS;
	return Status::BT_RUNNING;
}

