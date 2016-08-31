///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: If the AIEntity within the class has a target to track, the destination will be set to it.
//              If the AIEntity does NOT have a target, the destination will be set to the next path position.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "AI\BehaviorTrees\Action\SetDestinationBehavior.h"
#include "../../../../ProtoTowers/Components/AIEntity.h"
#include "Components\NavAgent.h"
#include "GameObject.h"
#include "Components\Transform.h"
#include "Components\Waypoint.h"

SetDestinationBehavior::SetDestinationBehavior(AIEntity* _entity, std::string& _name)
	: ActionBehavior(_name)
	, m_pEntity(_entity)
{ }

SetDestinationBehavior::~SetDestinationBehavior()
{ }

Status SetDestinationBehavior::Update()
{
	NavAgent* agent = m_pEntity->gameObject->GetComponent<NavAgent>();

	if (m_pEntity->IsTrackingEnemy())
		agent->SetDestination(m_pEntity->GetEnemyToTrack()->transform->GetPosition());
	else
		agent->SetDestination(agent->GetWaypoint()->transform->GetPosition());

	return Status::BT_SUCCESS;
}

void SetDestinationBehavior::Destroy()
{ }

