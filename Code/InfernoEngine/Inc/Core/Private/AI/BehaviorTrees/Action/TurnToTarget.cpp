///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: Makes an AIEntity turn to its target. Not error checked - MUST HAVE A TARGET.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "../Public/AI/BehaviorTrees/Action/TurnToTarget.h"
#include "Components\NavAgent.h"
#include "../../../../ProtoTowers/Components/AIEntity.h"
#include "Components\Transform.h"
#include "InfernoTime.h"

TurnToTarget::TurnToTarget(AIEntity* _entity, NavAgent* _agent, std::string& _name)
	: ActionBehavior(_name)
	, m_pAgent(_agent)
	, m_pEntity(_entity)
{ }

TurnToTarget::~TurnToTarget()
{ }

void TurnToTarget::Destroy()
{ }

Status TurnToTarget::Update()
{
	if (m_pEntity->GetEnemyToTrack() == nullptr)
		return Status::BT_FAILURE;

	Vector3 toPosition = m_pEntity->GetEnemyToTrack()->transform->GetPosition() - m_pEntity->transform->GetPosition();
	toPosition.Normalize();

	float dot = toPosition * m_pEntity->transform->Forward();

	if (dot < 0.999999f)
	{
		/// Turn right
		if (toPosition * m_pEntity->transform->Right() < 0)
			m_pEntity->transform->Rotate(Vector3(0, -ToRadians(m_pAgent->GetTurnRate()), 0) * Time::DeltaTime);
		/// Turn left
		else
			m_pEntity->transform->Rotate(Vector3(0, ToRadians(m_pAgent->GetTurnRate()), 0) * Time::DeltaTime);
	}

	return Status::BT_SUCCESS;
}

