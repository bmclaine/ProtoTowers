///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: Charge at target until you're within range OR a distance has been met.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "../Public/AI/BehaviorTrees/Action/ChargeAtTargetBehavior.h"
#include "Components\NavAgent.h"
#include "../../../../ProtoTowers/Components/AIEntity.h"
#include "Components\Transform.h"

ChargeAtTargetBehavior::ChargeAtTargetBehavior(NavAgent* _agent, std::string& _name)
	: ActionBehavior(_name)
	, m_pAgent(_agent)
{ }

ChargeAtTargetBehavior::~ChargeAtTargetBehavior()
{ }

void ChargeAtTargetBehavior::Destroy()
{ }

Status ChargeAtTargetBehavior::Update()
{
	m_pAgent->ChargeToTarget();
	return Status::BT_SUCCESS;
}

