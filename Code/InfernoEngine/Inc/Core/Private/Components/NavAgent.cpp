///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author:      Doug Berg
//
// Description: AI component that hadles path planning and movement
//              of the GameObject it's attached to.
//              This component communicates with the AI module to request a path between
//                   itself and a target position.
//
///////////////////////////////////////////////////////////////////////////////////////////////////


// STL Includes
#include <algorithm>
using namespace std;

#include <DirectXMath.h>
using namespace DirectX;

// Engine Includes
#include "Components\NavAgent.h"
#include "GameObject.h"
#include "Components\Transform.h"
#include "InfernoTime.h"
#include "../../Interface/Public/ModuleInterface.h"
#include "GlobalIncludes.h"
#include "Components\Waypoint.h"
#include "MathLib.h"
#include "../../../../ProtoTowers/Components/AIEntity.h"

// Defaults
NavAgent::NavAgent(GameObject* const _gameObject, Transform* const _transform)
	: AIComponent(_gameObject, _transform)
	, m_pWaypoint(nullptr)
	, m_uiMinionType(MinionType::NO_TYPE)
	, m_bIgnorePathing(false)
	, m_bPathFlag(true)
	, m_eLane(NO_LANE)
	, m_entity(nullptr)
{
	InfernoEngine_AI::RegisterNavAgent(this);
}

NavAgent::~NavAgent()
{
	InfernoEngine_AI::UnregisterNavAgent(this);
}

IComponent& NavAgent::operator=(IComponent& _rhs)
{
	NavAgent* rhsNavAgent = (NavAgent*)&_rhs;
	if (rhsNavAgent == nullptr)
		return *this;

	return operator=(*rhsNavAgent);
}

NavAgent& NavAgent::operator=(const NavAgent& _assign)
{
	if (this != &_assign)
	{
		m_vDestination   = _assign.m_vDestination;
		m_fSpeed         = _assign.m_fSpeed;
		m_fAcceleration  = _assign.m_fAcceleration;
		m_fRadius        = _assign.m_fRadius;
		m_fHeight        = _assign.m_fHeight;
		m_fTurnRate      = _assign.m_fTurnRate;
		m_path           = _assign.m_path;
		m_bPathFlag      = _assign.m_bPathFlag;
		m_bIgnorePathing = _assign.m_bIgnorePathing;
		m_uiMinionType   = _assign.m_uiMinionType;
		m_eLane          = _assign.m_eLane;
		m_entity         = _assign.m_entity;
		m_pWaypoint      = _assign.m_pWaypoint;

		m_fcFlags        = _assign.m_fcFlags;
	}

	return *this;
}

void NavAgent::OnEnable() /*override*/
{
	// Call Parent
	IComponent::OnEnable();

	m_entity = gameObject->GetComponent<AIEntity>();
}

IComponent* NavAgent::CreateCopy(GameObject* const _gameObject, Transform* const _transform)
{
	NavAgent* temp = new NavAgent(_gameObject, _transform);
	*temp = *this;

	return temp;
}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes object variables
void NavAgent::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{
	ObjectData* data = dataMap.find(GetInstanceID())->second;

	std::string propertyName = "Position";
	Property* prop = data->GetProperty(propertyName);
	std::vector<std::string> values = Inferno::StringParse(prop->value, ',');
	SetDestination(Vector3((float)atof(values[0].c_str()), (float)atof(values[1].c_str()), (float)atof(values[2].c_str())));

	propertyName = "Speed";
	prop = data->GetProperty(propertyName);
	m_fSpeed = (float)atof(prop->value.c_str());

	propertyName = "Acceleration";
	prop = data->GetProperty(propertyName);
	m_fAcceleration = (float)atof(prop->value.c_str());

	propertyName = "Radius";
	prop = data->GetProperty(propertyName);
	m_fRadius = (float)atof(prop->value.c_str());

	propertyName = "Height";
	prop = data->GetProperty(propertyName);
	m_fHeight = (float)atof(prop->value.c_str());

	propertyName = "TurnRate";
	prop = data->GetProperty(propertyName);
	m_fTurnRate = (float)atof(prop->value.c_str());

	propertyName = "PathFlag";
	prop = data->GetProperty(propertyName);
	m_bPathFlag = atoi(prop->value.c_str()) != 0;

	propertyName = "IgnorePath";
	prop = data->GetProperty(propertyName);
	m_bIgnorePathing = atoi(prop->value.c_str()) != 0;

	propertyName = "MinionType";
	prop = data->GetProperty(propertyName);
	SetMinionType((MinionType)atoi(prop->value.c_str()));

	propertyName = "Lane";
	prop = data->GetProperty(propertyName);
	m_eLane = (Lane)atoi(prop->value.c_str());

	m_bPathFlag = true;
}

// IN: Vector3 - The enemy position
//
// OUT: VOID
//
// Description: The agent will try to move to this position directly, without navigation
void NavAgent::MoveToDestination()
{
	if (m_entity->GetState() == State::STUNNED)
		return;
	if (m_entity->GetType() == AIEntityType::TYPE_TANK && m_entity->GetEnemyToTrack() != nullptr)
		return;

	if (m_path.size() != 0)
	{
		TurnTo(m_path.back());
		transform->SetPosition(transform->GetPosition() + transform->Forward() * m_fSpeed * Time::DeltaTime);

		float distanceToPath = (m_path.back() - transform->GetPosition()).MagnitudeSq();
		if (distanceToPath < 5.0f * 5.0f)
			m_path.erase(m_path.end() - 1);

		float distanceToWaypoint = (m_pWaypoint->transform->GetPosition() - transform->GetPosition()).MagnitudeSq();
		if (distanceToWaypoint < 5.0f * 5.0f && m_pWaypoint->GetNextWaypoint() != nullptr)
			m_pWaypoint = m_pWaypoint->GetNextWaypoint();
	}
	else if (m_entity->IsRunning())
	{
		TurnTo(m_vDestination);
		transform->SetPosition(transform->GetPosition() + transform->Forward() * m_fSpeed * Time::DeltaTime);
	}
}

void NavAgent::MoveToWaypoint()
{
	if (m_entity->GetState() == State::STUNNED)
		return;

	if (m_entity != nullptr && m_entity->IsRunning())
	{
		TurnTo(Vector3(m_pWaypoint->transform->GetPosition().x, 0, m_pWaypoint->transform->GetPosition().z));
		transform->SetPosition(transform->GetPosition() + transform->Forward() * m_fSpeed * Time::DeltaTime);
	}
	
	float distanceToPath = (transform->GetPosition() - m_path.back()).LengthSq();
	if (distanceToPath < 5.0f * 5.0f)
		m_path.erase(m_path.end() - 1);

	float distanceToWaypoint = (m_pWaypoint->transform->GetPosition() - transform->GetPosition()).MagnitudeSq();
	if (distanceToWaypoint < 3.0f * 3.0f && m_pWaypoint->GetNextWaypoint() != nullptr)
		m_pWaypoint = m_pWaypoint->GetNextWaypoint();
}

void NavAgent::MoveToTarget()
{
	if (m_entity->GetState() == State::STUNNED)
		return;

	if (m_entity != nullptr && m_entity->IsRunning())
	{
		TurnTo(Vector3(m_vDestination.x, 0, m_vDestination.z));
		transform->SetPosition(transform->GetPosition() + transform->Forward() * m_fSpeed * Time::DeltaTime);
	}
}

void NavAgent::ChargeToTarget()
{
	transform->SetPosition(transform->GetPosition() + transform->Forward() * (m_fSpeed * 2.5f) * Time::DeltaTime);
}

// IN: VOID
//
// OUT: VOID
//
// Description: Moves the agent by its speed
void NavAgent::Update()
{ /* Handled by behavior trees*/ }

// IN: _waypoint - The waypoint to add
//
// OUT: VOID
//
// Description: Adds a waypoint to the agents path
void NavAgent::SetWaypoint(Waypoint* _waypoint)
{
	ASSERT(_waypoint != nullptr, "Attempting to set NavAgents next waypoint to nullptr.");
	m_pWaypoint = _waypoint;
}

// IN: bool - The new bool
//
// OUT: VOID
//
// Description: Sets the agent to ignore the pathfinding or not
void NavAgent::SetIgnorePath(bool _ignore)
{
	m_bIgnorePathing = _ignore;
}

// IN: _path - The path to set the agent to
//
// OUT: VOID
//
// Description: The agent will attempt to follow the given path
void NavAgent::SetPath(vector<Vector3>& _path)
{
	m_path = _path;
}

// IN: Vector3 - The desired destination location
//
// OUT: VOID
//
// Description: Sets the agent's destination to the parameter
void NavAgent::SetDestination(Vector3 _destination)
{
	m_vDestination = _destination;
}

// IN: float - The desired speed
//
// OUT: VOID
//
// Description: Sets the agent's speed to the parameter
void NavAgent::SetSpeed(float _speed)
{
	m_fSpeed = _speed;
}

// IN: float - The desired acceleration
//
// OUT: VOID
//
// Description: Sets the agent's acceleration to the parameter
void NavAgent::SetAcceleration(float _accel)
{
	m_fAcceleration = _accel;
}

// IN: float - The desired radius
//
// OUT: VOID
//
// Description: Sets the agent's radius to the parameter
void NavAgent::SetAgentRadius(float _radius)
{
	m_fRadius = _radius;
}

// IN: float - The desired height
//
// OUT: VOID
//
// Description: Sets the agent's height to the parameter
void NavAgent::SetAgentHeight(float _height)
{
	m_fHeight = _height;
}

// IN: bool - The new boolean
//
// OUT: VOID
//
// Description: If this flag is true, the agent is requesting a new path
void NavAgent::SetPathFlag(bool _pathFlag)
{
	m_bPathFlag = _pathFlag;
}

// IN: float - The new turn rate
//
// OUT: VOID
//
// Description: Sets the turn rate for the agent
void NavAgent::SetTurnRate(float _rate)
{
	m_fTurnRate = _rate;
}

// IN: MinionType - The type of minion
//
// OUT: VOID
//
// Description: This is used to determine the type of agent behavior based on the minion type
void NavAgent::SetMinionType(MinionType _type)
{
	ASSERT(_type >= 0 && _type < MINION_TYPE_MAX, "MinionType value is out of range");
	m_uiMinionType = _type;

	
	switch (m_uiMinionType)
	{
	case MINI:
		m_fAcceleration = 1.0f;
		m_fHeight       = 1.0f;
		m_fRadius       = 1.0f;
		m_fSpeed        = 4.2f;
		m_fTurnRate     = 540;
		break;

	case RANGED:
		m_fAcceleration = 1.0f;
		m_fHeight       = 1.0f;
		m_fRadius       = 1.0f;
		m_fSpeed        = 1.0f;
		m_fTurnRate     = 180;
		break;

	case TANK:
		m_fAcceleration = 1.0f;
		m_fHeight       = 3.0f;
		m_fRadius       = 2.0f;
		m_fSpeed        = 4.0f;
		m_fTurnRate     = 360;
		break;

	case BOSS:
		m_fAcceleration = 1.0f;
		m_fHeight       = 4.0f;
		m_fRadius       = 3.0f;
		m_fSpeed        = 6.0f;
		m_fTurnRate     = 240;
		break;
	}
}

// IN: Lane - The lane for the agent to follow
//
// OUT: VOID
//
// Description: Sets the agent to follow the specified lane
void NavAgent::SetLane(Lane _lane)
{
	m_eLane = _lane;
}

// IN: VOID
//
// OUT: stack<Vector4>& - The agents chosen path
//
// Description: Returns the path that the agent will be traversing
const vector<Vector3>& NavAgent::GetPath() const
{
	return m_path;
}
vector<Vector3>& NavAgent::GetPath()
{
	return m_path;
}

// IN: VOID
//
// OUT: bool - If the agent is ignoring pathfinding
//
// Description: Returns true if the agent is ignoring pathfinding
bool NavAgent::IsIgnoringPathfinding() const
{
	return m_bIgnorePathing;
}

// IN: VOID
//
// OUT: Vector3 - The agents destination
//
// Description: Returns the location of where the agent is trying to get to
const Vector3& NavAgent::GetDestination() const
{
	return m_vDestination;
}
Vector3 NavAgent::GetDestination()
{
	return m_vDestination;
}

// IN: VOID
//
// OUT: Vector3 - The waypoints
//
// Description: Returns the agents waypoints
const Waypoint* NavAgent::GetWaypoint() const
{
	return m_pWaypoint;
}
Waypoint* NavAgent::GetWaypoint()
{
	return m_pWaypoint;
}

// IN: VOID
//
// OUT: float - The speed of the agent
//
// Description: Returns the current speed of the agent
float NavAgent::GetSpeed()
{
	return m_fSpeed;
}

// IN: VOID
//
// OUT: float - The radius of the agent
//
// Description: Returns the current radius of the agent
float NavAgent::GetAgentRadius()
{
	return m_fRadius;
}

// IN: VOID
//
// OUT: float - The height of the agent
//
// Description: Returns the current height of the agent
float NavAgent::GetAgentHeight()
{
	return m_fHeight;
}

// IN: VOID
//
// OUT: bool - Boolean for the agent's path flag
//
// Description: Returns true if this agent needs a path, false otherwise
bool NavAgent::GetPathFlag() const
{
	return m_bPathFlag;
}

// IN: VOID
//
// OUT: float - The turn rate of the agent
//
// Description: Returns the turn rate of the agent
float NavAgent::GetTurnRate() const
{
	return m_fTurnRate;
}

// IN: VOID
//
// OUT: MinionType - The type of agent behavior
//
// Description: Returns the type of behavior this agent has
unsigned int NavAgent::GetMinionType() const
{
	return m_uiMinionType;
}

// IN: VOID
//
// OUT: Lane - The lane the agent if following
//
// Description: Returns the lane that the agent is assigned to
Lane NavAgent::GetLane() const
{
	return m_eLane;
}

void NavAgent::TurnTo(Vector3& _position)
{
	Vector3 toPosition = _position - transform->GetPosition();
	toPosition.Normalize();

	float dot = toPosition * transform->Forward();

	if (dot < 0.9f)
	{
		if (toPosition * transform->Right() < 0)	// Turn Right
			transform->Rotate(Vector3(0, -ToRadians(m_fTurnRate), 0) * Time::DeltaTime);
		else // Turn Left
			transform->Rotate(Vector3(0, ToRadians(m_fTurnRate), 0) * Time::DeltaTime);
	}
}

