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
#pragma once


// STL Includes
#include <vector>
#include <map>

// Engine Includes
#include "AIComponent.h"
#include "MathLib.h"


// Forward Declaration
class GameObject;
class Transform;
class Waypoint;
class AIEntity;

enum MinionType{ NO_TYPE, MINI, RANGED, TANK, BOSS,
	MINION_TYPE_MAX };

enum Lane { LEFT, RIGHT, NO_LANE,
	LANE_MAX };

class NavAgent : public AIComponent
{
private:
	Vector3                m_vDestination;     // -> The location that the agent is trying to get to
	float                  m_fSpeed;           // -> The speed of the agent
	float                  m_fAcceleration;    // -> The acceleration of the agent
	float                  m_fRadius;          // -> The radius of the agent
	float                  m_fHeight;          // -> The height of the agent
	float                  m_fTurnRate;        // -> The turn rate of the agent
	std::vector < Vector3 >m_path;             // -> The current path of the agent. The first index will be the 'next' location
	bool                   m_bPathFlag;        // -> A flag representing if the agent needs a path or not
	bool                   m_bIgnorePathing;   // -> If true, ignores pathfinding
	unsigned int           m_uiMinionType;     // -> The type of minion(i.e. Mini, Ranged, Boss, etc.)
	Lane                   m_eLane;            // -> The lane that the minion is travelling on
	AIEntity              *m_entity;           // -> The entity on the gameObject
	
	Waypoint* m_pWaypoint;

	IComponent* CreateCopy(GameObject* const _gameObject, Transform* const _transform);

	void TurnTo(Vector3& _position);

	// IN: _path - The path to set the agent to
	//
	// OUT: VOID
	//
	// Description: The agent will attempt to follow the given path
	void SetPath(std::vector< Vector3 >& _path);

	// IN: float - The desired acceleration
	//
	// OUT: VOID
	//
	// Description: Sets the agent's acceleration to the parameter
	void SetAcceleration(float _accel);

	// IN: float - The desired radius
	//
	// OUT: VOID
	//
	// Description: Sets the agent's radius to the parameter
	void SetAgentRadius(float _radius);

	// IN: float - The desired height
	//
	// OUT: VOID
	//
	// Description: Sets the agent's height to the parameter
	void SetAgentHeight(float _height);

	// IN: float - The new turn rate
	//
	// OUT: VOID
	//
	// Description: Sets the turn rate for the agent
	void SetTurnRate(float _rate);


public:
	// Defaults
	NavAgent(GameObject* const _gameObject, Transform* const _transform);
	~NavAgent( void );

	virtual IComponent& operator=(IComponent& _rhs) override;
	NavAgent& operator=(const NavAgent& _assign);
	void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& _dataMap);
	void OnEnable( void ) override;

	// IN: Vector3 - The enemy position
	//
	// OUT: VOID
	//
	// Description: The agent will try to move to this position directly, without navigation
	void MoveToDestination( void );
	void MoveToWaypoint( void );
	void MoveToTarget( void );
	void ChargeToTarget( void );

	// IN: VOID
	//
	// OUT: VOID
	//
	// Description: Moves the agent by its speed
	virtual void Update( void ) override;

	// IN: bool - The new bool
	//
	// OUT: VOID
	//
	// Description: Sets the agent to ignore the pathfinding or not
	void SetIgnorePath(bool _ignore);

	// IN: Vector3 - The desired destination location
	//
	// OUT: VOID
	//
	// Description: Sets the agent's destination to the parameter
	void SetDestination(Vector3 _destination);

	// IN: _waypoint - The waypoint to add
	//
	// OUT: VOID
	//
	// Description: Adds a waypoint to the agents path
	void SetWaypoint(Waypoint* _waypoint);

	// IN: bool - The new boolean
	//
	// OUT: VOID
	//
	// Description: If this flag is true, the agent is requesting a new path
	void SetPathFlag(bool _pathFlag);

	// IN: MinionType - The type of minion
	//
	// OUT: VOID
	//
	// Description: This is used to determine the type of agent behavior based on the minion type
	void SetMinionType(MinionType _type);

	// IN: Lane - The lane for the agent to follow
	//
	// OUT: VOID
	//
	// Description: Sets the agent to follow the specified lane
	void SetLane(Lane _lane);

	// IN: float - The desired speed
	//
	// OUT: VOID
	//
	// Description: Sets the agent's speed to the parameter
	void SetSpeed(float _speed);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// IN: VOID
	//
	// OUT: stack<Vector4>& - The agents chosen path
	//
	// Description: Returns the path that the agent will be traversing
	const std::vector<Vector3>& GetPath( void ) const;
	std::vector<Vector3>& GetPath(void);

	// IN: VOID
	//
	// OUT: bool - If the agent is ignoring pathfinding
	//
	// Description: Returns true if the agent is ignoring pathfinding
	bool IsIgnoringPathfinding( void ) const;

	// IN: VOID
	//
	// OUT: float - The speed of the agent
	//
	// Description: Returns the current speed of the agent
	float GetSpeed ( void );

	// IN: VOID
	//
	// OUT: Vector3 - The agents destination
	//
	// Description: Returns the location of where the agent is trying to get to
	const Vector3& GetDestination( void ) const;
	Vector3 GetDestination(void);

	// IN: VOID
	//
	// OUT: Vector3 - The waypoints
	//
	// Description: Returns the agents waypoints
	const Waypoint* GetWaypoint( void ) const;
	Waypoint* GetWaypoint( void );

	// IN: VOID
	//
	// OUT: float - The radius of the agent
	//
	// Description: Returns the current radius of the agent
	float GetAgentRadius( void );

	// IN: VOID
	//
	// OUT: float - The height of the agent
	//
	// Description: Returns the current height of the agent
	float GetAgentHeight( void );

	// IN: VOID
	//
	// OUT: bool - Boolean for the agent's path flag
	//
	// Description: Returns true if this agent needs a path, false otherwise
	bool GetPathFlag( void ) const;

	// IN: VOID
	//
	// OUT: float - The turn rate of the agent
	//
	// Description: Returns the turn rate of the agent
	float GetTurnRate( void ) const;

	// IN: VOID
	//
	// OUT: MinionType - The type of agent behavior
	//
	// Description: Returns the type of behavior this agent has
	unsigned int GetMinionType( void ) const;

	// IN: VOID
	//
	// OUT: Lane - The lane the agent if following
	//
	// Description: Returns the lane that the agent is assigned to
	Lane GetLane(void) const;
};

