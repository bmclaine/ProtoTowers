///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: Makes an entity look at its target.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "../Public/AI/BehaviorTrees/Action/LookAtBehavior.h"
#include "../../../../ProtoTowers/Components/AIEntity.h"
#include "Components\NavAgent.h"
#include "Components\Transform.h"
#include "InfernoTime.h"

LookAtBehavior::LookAtBehavior(AIEntity* _entity, NavAgent* _agent, std::string& _name)
	: ActionBehavior(_name)
	, m_pEntity(_entity)
	, m_pAgent(_agent)
{ }

LookAtBehavior::~LookAtBehavior()
{ }

void LookAtBehavior::Destroy()
{ }

Status LookAtBehavior::Update()
{
	if (m_pEntity->GetEnemyToTrack() == nullptr)
		return Status::BT_FAILURE;

	float dot = FLT_MAX;

	do
	{
		Vector3 toPosition = m_pEntity->GetEnemyToTrack()->transform->GetPosition() - m_pEntity->transform->GetPosition();
		toPosition.Normalize();

		float dot = toPosition * m_pEntity->transform->Forward();

		/// Turn right
		if (toPosition * m_pEntity->transform->Right() < 0)
			m_pEntity->transform->Rotate(Vector3(0, -ToRadians(m_pAgent->GetTurnRate()), 0) * Time::DeltaTime);
		/// Turn left
		else
			m_pEntity->transform->Rotate(Vector3(0, ToRadians(m_pAgent->GetTurnRate()), 0) * Time::DeltaTime);

	} while (dot < 0.99f);

	return Status::BT_SUCCESS;
}

