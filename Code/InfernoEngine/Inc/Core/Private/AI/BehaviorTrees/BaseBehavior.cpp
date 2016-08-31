///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: BaseBehavior is the base class of ALL behaviors.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "../Public/AI/BehaviorTrees/BaseBehavior.h"


// Defaults
BaseBehavior::BaseBehavior(std::string& _name)
	: m_sName(_name)
	, m_eStatus(Status::BT_INVALID)
{ }

BaseBehavior::~BaseBehavior()
{ }

// Interface
Status BaseBehavior::Tick()
{
	if (m_eStatus != Status::BT_RUNNING)
		Initialize();
	
	m_eStatus = Update();

	if (m_eStatus != Status::BT_RUNNING)
		OnTerminate(m_eStatus);

	return m_eStatus;
}

void BaseBehavior::Initialize()
{
	m_eStatus = Status::BT_RUNNING;
}

void BaseBehavior::OnTerminate(Status _status)
{ }

void BaseBehavior::Abort()
{
	OnTerminate(Status::BT_ABORTRED);
	m_eStatus = Status::BT_ABORTRED;
}

void BaseBehavior::Reset()
{
	m_eStatus = Status::BT_INVALID;
}

// Accessors
bool BaseBehavior::IsTerminated() const
{
	return m_eStatus == Status::BT_SUCCESS || m_eStatus == Status::BT_FAILURE;
}

bool BaseBehavior::IsRunning() const
{
	return m_eStatus == Status::BT_RUNNING;
}

Status BaseBehavior::GetStatus() const
{
	return m_eStatus;
}

BehaviorType BaseBehavior::GetType() const
{
	return BehaviorType::BASE;
}

std::string BaseBehavior::GetName() const
{
	return m_sName;
}

// Mutators
void BaseBehavior::SetName(std::string _name)
{
	m_sName = _name;
}

