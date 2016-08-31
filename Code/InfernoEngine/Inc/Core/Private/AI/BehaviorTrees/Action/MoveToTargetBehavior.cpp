///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: BehaviorTree is the class that will be the base of everything.
//              It will have one BaseBehavior child that will act as the Tree's root.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "AI\BehaviorTrees\Action\MoveToTargetBehavior.h"
#include "Components\NavAgent.h"
#include "GameObject.h"
#include "../../../../ProtoTowers/Components/AIEntity.h"
#include "Components\Transform.h"

MoveToTargetBehavior::MoveToTargetBehavior(NavAgent* _agent, std::string& _name)
	: ActionBehavior(_name)
	, m_pAgent(_agent)
{ }

MoveToTargetBehavior::~MoveToTargetBehavior()
{ }

void MoveToTargetBehavior::Destroy()
{ }

Status MoveToTargetBehavior::Update()
{
	ASSERT(m_pAgent != nullptr, "MoveToTargetBehavior agent is NULL.");

	if (m_pAgent->gameObject->GetComponent<AIEntity>()->IsTrackingEnemy() == false)
		return Status::BT_FAILURE;

	AIEntity* entity = m_pAgent->gameObject->GetComponent<AIEntity>();
	Entity* tracking = entity->GetEnemyToTrack();
//	m_pAgent->SetDestination(m_pAgent->gameObject->GetComponent<AIEntity>()->GetEnemyToTrack()->transform->GetPosition());

	m_pAgent->MoveToDestination();

	return Status::BT_SUCCESS;
}

