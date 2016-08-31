///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: Sets a NavAgents path flag to true.
//              A new path will be calculated on the next frame, by AIModule.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "AI\BehaviorTrees\Action\RequestPathBehavior.h"
#include "Components\NavAgent.h"
#include "../../../../ProtoTowers/Components/AIEntity.h"
#include "GameObject.h"

RequestPathBehavior::RequestPathBehavior(NavAgent* _agent, std::string& _name)
	: ActionBehavior(_name)
	, m_pAgent(_agent)
{ }

RequestPathBehavior::~RequestPathBehavior()
{ }

Status RequestPathBehavior::Update()
{
	AIEntity *entity = m_pAgent->gameObject->GetComponent<AIEntity>();
	ASSERT(entity != nullptr, "Requesting path for a gameObject without an AIEntity attached");
//	if (entity->IsTrackingEnemy() == false)
		m_pAgent->SetPathFlag(true);
	return Status::BT_SUCCESS;
}

void RequestPathBehavior::Destroy()
{ }

