///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: This will call the entities function to find its closest target, if any.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "AI\BehaviorTrees\Action\FindClosestTargetBehavior.h"
#include "../../../../ProtoTowers/Components/AIEntity.h"

FindClosestTargetBehavior::FindClosestTargetBehavior(AIEntity* _entity, std::string& _name)
	: ActionBehavior(_name)
	, m_pEntity(_entity)
{ }

FindClosestTargetBehavior::~FindClosestTargetBehavior()
{ }

Status FindClosestTargetBehavior::Update()
{
	ASSERT(m_pEntity != nullptr, "FindClosestTargetBehavior::Update() -> Entity is nullptr");

	m_pEntity->FindClosestTarget();

	return Status::BT_SUCCESS;
}

void FindClosestTargetBehavior::Destroy()
{ }