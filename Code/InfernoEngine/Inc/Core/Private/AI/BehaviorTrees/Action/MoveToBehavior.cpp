///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: MoveToBehavior will move an entity to a specified location.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "AI\BehaviorTrees\Action\MoveToBehavior.h"
#include "Components\NavAgent.h"
#include "../../../../ProtoTowers/Components/AIEntity.h"
#include "GameObject.h"

MoveToBehavior::MoveToBehavior(NavAgent* _agent, std::string& _name)
	: BaseBehavior(_name)
	, m_pAgent(_agent)
{ }

MoveToBehavior::~MoveToBehavior()
{ }

void MoveToBehavior::Destroy()
{ }

Status MoveToBehavior::Update()
{
	m_pAgent->MoveToDestination();
	return Status::BT_SUCCESS;
}

