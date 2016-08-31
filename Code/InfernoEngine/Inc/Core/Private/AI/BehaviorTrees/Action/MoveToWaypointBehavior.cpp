///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: Will move the attached NavAgent along its path,
//					which has already been calculated to find its waypoint.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "AI\BehaviorTrees\Action\MoveToWaypointBehavior.h"
#include "Components\NavAgent.h"
#include "Components\Transform.h"
#include "Components\Waypoint.h"
#include "MathLib.h"

MoveToWaypointBehavior::MoveToWaypointBehavior(NavAgent* _agent, std::string& _name)
	: ActionBehavior(_name)
	, m_pAgent(_agent)
{ }

MoveToWaypointBehavior::~MoveToWaypointBehavior()
{ }

void MoveToWaypointBehavior::Destroy()
{ }

Status MoveToWaypointBehavior::Update()
{
	// If we've made it this far in the tree, we've requested a path
	// path.size() == 0 means that the path is still being calculated
	if (m_pAgent->GetPath().size() == 0)
		return Status::BT_RUNNING;

	m_pAgent->MoveToWaypoint();

	return Status::BT_SUCCESS;
}
