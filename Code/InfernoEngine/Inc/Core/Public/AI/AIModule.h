///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: The AIModule class brings all the AI components together.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

// STL
#include <queue>
#include <vector>

// Engine includes
#include "ThreadPool\ThreadPool.h"
#include "AI\NavMesh.h"
#include "MathLib.h"

#define DISTANCE_SQR(x) (x * x)

// Forward Declaration
class NavAgent;
class Mesh;
class PathfindingThread;
class NavMesh;
class GameObject;
class AIEntity;

struct Request
{
	NavAgent*               agent;      // The agent that made the request
	Vector3                 start, end;
	std::vector< Vector3 >* path;
	NavMesh*                navMesh;    // The navMesh that the agent will be traversing
	Request(NavAgent* _agent, Vector3& _start, Vector3& _end) :
		agent( _agent ),
		start( _start ),
		end  ( _end   )
	{ }
};

class AIModule
{
private:
	ThreadPool                        m_threadPool;             // -> The only thread pool in the entire program. Used to share the work load of all the AI pathfinding.
	NavMesh                           m_navMesh;                // -> The levels NavMesh

	std::vector< Request* >           m_freeRequests;           // -> A pool of free requests to pull from, rather than calling 'new' during run-time
	std::vector< PathfindingThread* > m_freePathfindingThreads; // -> A pool of free pathfinding threads to pull from, rather than calling 'new' during run-time

	std::vector< Request* >           m_requests;               // -> A FIFO queue of Requests from the NavAgent. All of these are to be processed once a thread is available.
	std::vector< Request* >           m_processing;             // -> A list of requests that have been sent to the threadPool and are waiting to be completed.

	std::vector< NavAgent* >          m_vNavAgents;             // -> NavAgents in the program

	std::vector< GameObject* >        m_enemies;                // -> The enemies to be tracked
	std::vector< AIEntity* >          m_AIEntities;             // -> The AI entities

	// IN: VOID
	//
	// OUT: VOID
	//
	// Description: Updates everything to do with pathfinding, tracking the pathfinding, and requests
	void UpdatePathfinding( void );

	// IN: NavAgent - A pointer to the NavAgent that will be making this function call.
	//     Vector3  - The start point of the pathfinding
	//     Vector3  - The goal point of the pathfinding
	//
	// OUT: VOID
	//
	// Description: A NavAgent will call this function when it needs to calculate pathfinding.
	//              The parameters will be inserted into a Request structure and then added into
	//                   a queue to be processed by the next open thread in the thread pool.
	void RequestPath(NavAgent* _agent);

public:
	// Defaults
	AIModule ( void );
	~AIModule( void );
	
	// IN: VOID
	//
	// OUT: VOID
	//
	// Description: Returns completed paths to the correct NavAgent that requested it.
	void Update( void );
	void FixedUpdate( void );

	// IN: VOID
	//
	// OUT: VOID
	//
	// Description: Draws the NavMesh outlines
	void OnGizmoDraw( void );

	// IN: Mesh* - The mesh to have a navigation area loaded from
	//
	// OUT: BOOL - Returns true if the loading was successful and false otherwise.
	//
	// Description: The parameter Mesh is used to load a navigation area.
	bool LoadNavMesh(Mesh* _navMesh, Matrix4& _globalPosition);

	// IN: AIEntity* - The entity to register
	//
	// OUT: bool - Result of registry
	//
	// Description: Returns true if the register was success
	bool RegisterAIEntity(AIEntity* _entity);

	// IN: AIEntity* - The AI entity to unregister
	//
	// OUT: bool - True if unregister success
	//
	// Description: 
	bool UnregisterAIEntity(AIEntity* _entity);

	// IN: _agent* - The agent to be registered
	//
	// OUT: bool - True if registered success
	//
	// Description: Registers a NavAgent to the invoking AIModule
	bool RegisterAgent(NavAgent* _agent);

	// IN: _agent* - The agent to unregister
	//
	// OUT: bool - True if registered success
	//
	// Description: Unregisters a NavAgent from the invoking object
	bool UnregisterAgent(NavAgent* _agent);

	// IN: GameObject* - The player
	//
	// OUT: bool - True if registered success
	//
	// Description: Registers the player to the invoking AIModule
	bool RegisterPlayer(GameObject* _player);

	// IN: GameObject* - The player
	//
	// OUT: bool - True if the unregister was success
	//
	// Description: Unregisters the player to the invoking AIModule
	bool UnregisterPlayer(GameObject* _player);

	void EnterCutscene();
	void ExitCutscene();
};

