///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: The AIModule class brings all the AI components together.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#define NUM_REQUESTS				300
#define NUM_PATHFINDING_THREADS		300

// STL Includes
#include <algorithm>
using namespace std;

// Engine Includes
#include "AI\AIModule.h"
#include "../../Interface/Public/Engine.h"
#include "..\Public\ThreadPool\PathfindingThread.h"
#include "GameObject.h"
#include "Assets\Mesh.h"
#include "../../../../ProtoTowers/Components/AIEntity.h"
#include "../../../../ProtoTowers/Components/BossMinion.h"
/// Tools
#include "InfernoTime.h"
#include "Gizmos.h"
/// Components
#include "Components\Transform.h"
#include "Components\NavAgent.h"
#include "Components\Waypoint.h"
#include "Components\BehaviorTree.h"

// Defaults
AIModule::AIModule()
{
	// Requests
	for (unsigned int i = 0; i < NUM_REQUESTS; ++i)
		m_freeRequests.push_back(new Request(nullptr, Vector3::Zero(), Vector3::Zero()));

	// Pathfinding threads
	for (unsigned int i = 0; i < NUM_PATHFINDING_THREADS; ++i)
		m_freePathfindingThreads.push_back(new PathfindingThread());
}

AIModule::~AIModule()
{
	m_threadPool.Shutdown();

	for (unsigned int i = 0; i < m_freePathfindingThreads.size(); ++i)
		delete m_freePathfindingThreads[i];

	while (m_freeRequests.size())
	{
		Request* temp = m_freeRequests.back();
		delete temp;
		m_freeRequests.pop_back();
	}
	m_freeRequests.clear();
}

// IN: VOID
//
// OUT: VOID
//
// Description: Returns completed paths to the correct NavAgent that requested it.
void AIModule::Update()
{
	UpdatePathfinding();

	BehaviorTree* tree = nullptr;
	Status status = Status::BT_INVALID;

	unsigned int size = (unsigned int)m_AIEntities.size();
	for (unsigned int i = 0; i < size; ++i)
	{
		status = Status::BT_INVALID;
		if (m_AIEntities[i]->isEnabled() && m_AIEntities[i]->gameObject->ActiveInHierarchy())
		{
			tree = m_AIEntities[i]->gameObject->GetComponent<BehaviorTree>();
			if (tree != nullptr && tree->isEnabled())
				status = tree->Tick();

			/// Handle any requests that were made
			NavAgent* agent = m_AIEntities[i]->gameObject->GetComponent<NavAgent>();
			if (agent && agent->GetPathFlag() == true)
			{
				agent->SetPathFlag(false);
				RequestPath(agent);
			}
		}
	}
}

void AIModule::FixedUpdate()
{
	// To be implemented...
}

// IN: VOID
//
// OUT: VOID
//
// Description: Draws the NavMesh outlines
void AIModule::OnGizmoDraw()
{
	/// DeltaTime being greater than 0 represents that we're not in the main menu anymore
	if (Time::DeltaTime > 0)
	{
		Gizmos::SetColor(Color(0, 1.0f, 0, 1.0f)); /// Green
		Gizmos::DrawWireMesh(Inferno::GetMeshByName(m_navMesh.GetName().c_str()), Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 0.0f, 1.0f));
		/// Draw the agents' waypoint in red and their destination in yellow
		unsigned int size = (unsigned int)m_vNavAgents.size();
		for (unsigned int i = 0; i < size; ++i)
		{
			Gizmos::SetColor(Color(1.0f, 0, 0, 1.0f));/// Red Waypoint
			if (m_vNavAgents[i]->GetWaypoint() != nullptr)
				Gizmos::DrawWireSphere(m_vNavAgents[i]->GetWaypoint()->transform->GetPosition(), 0.2f);
			Gizmos::SetColor(Color(1.0f, 1.0f, 0, 1.0f));/// Yellow Destination
			Gizmos::DrawWireSphere(m_vNavAgents[i]->GetDestination(), 0.5f);
		}
	}
}

// IN: VOID
//
// OUT: VOID
//
// Description: Updates everything to do with pathfinding, tracking the pathfinding, and requests
void AIModule::UpdatePathfinding()
{
	PathfindingThread* thread = nullptr;

	Request* request = nullptr;

	while (m_requests.size() > 0)
	{
		request = m_requests.front();
		m_requests.erase(m_requests.begin());

		// A request needs to be processed
		// Grab a free pathfinding thread and send it to the thread pool
		ASSERT(m_freePathfindingThreads.size() > 0, "PathfindingThreadPool in AIModule is not large enough");

		thread = m_freePathfindingThreads.back();
		m_freePathfindingThreads.pop_back();

		thread->Initialize(request);

		m_processing.push_back(request);
		m_threadPool.PostWork(thread);

		/// Check the processing list for completed paths
		while (m_processing.size() > 0)
		{
			while (m_threadPool.HasAgentCompleted())
			{
				thread = m_threadPool.GetFirstCompleted();

				/// Give the thread back to the free pool
				m_freePathfindingThreads.push_back(thread);

				for (unsigned int i = 0; i < m_processing.size(); ++i)
				{
					if (m_processing[i]->agent == thread->GetRequest()->agent)
					{
						/// The processing is done - a path has been calculated
						m_freeRequests.push_back(thread->GetRequest());
						m_processing.erase(m_processing.begin() + i);
						break;
					}
				}
			}
		}
	}

#pragma region DEBUGGING_INFO
#if 0
	unsigned int active = 0;
	for (unsigned int i = 0; i < m_AIEntities.size(); ++i)
	{
		if (m_AIEntities[i]->isEnabled())
			++active;
	}

	std::string message("RequestPool.size() == ");
	message += to_string((unsigned int)m_requests.size());
	message += "\t\t";
	message += to_string(active);
	message += "\n";
	PRINT(message);
#endif
#pragma endregion
}

// IN: Mesh* - The mesh to have a navigation area loaded from
//
// OUT: BOOL - Returns true if the loading was successful and false otherwise.
//
// Description: The parameter Mesh is used to load a navigation agea.
bool AIModule::LoadNavMesh(Mesh* _navMesh, Matrix4& _globalPosition)
{
	return m_navMesh.Load(_navMesh, _globalPosition);
}

// IN: AIEntity* - The entity to register
//
// OUT: bool - Result of registry
//
// Description: Returns true if the register was success
bool AIModule::RegisterAIEntity(AIEntity* _entity)
{
	for (unsigned int i = 0; i < m_AIEntities.size(); ++i)
	{
		if (m_AIEntities[i] == _entity)
			return false;
	}
	m_AIEntities.push_back(_entity);
	return true;
}

// IN: AIEntity* - The AI entity to unregister
//
// OUT: bool - True if unregister success
//
// Description: 
bool AIModule::UnregisterAIEntity(AIEntity* _entity)
{
	for (unsigned int i = 0; i < m_AIEntities.size(); ++i)
	{
		if (m_AIEntities[i] == _entity)
		{
			m_AIEntities.erase(m_AIEntities.begin() + i);
			return true;
		}
	}
	return false;
}

// IN: _agent* - The agent to be registered
//
// OUT: VOID
//
// Description: Registers a NavAgent to the invoking AIModule
bool AIModule::RegisterAgent(NavAgent* _agent)
{
	for (int i = 0; i < m_vNavAgents.size(); ++i)
	{
		if (m_vNavAgents[i] == _agent)
			return false;
	}
	m_vNavAgents.push_back(_agent);
	return true;
}

// IN: _agent* - The agent to unregister
//
// OUT: VOID
//
// Description: Unregisters a NavAgent from the invoking object
bool AIModule::UnregisterAgent(NavAgent* _agent)
{
	for (unsigned int i = 0; i < m_vNavAgents.size(); ++i)
	{
		if (m_vNavAgents[i] == _agent)
		{
			m_vNavAgents.erase(m_vNavAgents.begin() + i);
			return true;
		}
	}
	return false;
}

// IN: GameObject* - The player
//
// OUT: bool - True if registered success
//
// Description: Registers the player to the invoking AIModule
bool AIModule::RegisterPlayer(GameObject* _player)
{
	m_enemies.push_back(_player);
	return true;
}

// IN: GameObject* - The player
//
// OUT: bool - True if the unregister was success
//
// Description: Unregisters the player to the invoking AIModule
bool AIModule::UnregisterPlayer(GameObject* _player)
{
	for (unsigned int i = 0; i < m_enemies.size(); ++i)
	{
		if (m_enemies[i] == _player)
		{
			m_enemies.erase(m_enemies.begin() + i);
			return true;
		}
	}
	return false;
}

// IN: NavAgent - A pointer to the NavAgent that will be making this function call.
//     Vector3  - The start point of the pathfinding
//     Vector3  - The goal point of the pathfinding
//
// OUT: VOID
//
// Description: A NavAgent will call this function when it needs to calculate pathfinding.
//              The parameters will be inserted into a Request structure and then added into
//                   a queue to be processed by the next open thread in the thread pool.
void AIModule::RequestPath(NavAgent* _agent)
{
#pragma region DEBUGGING_INFO
#if 0
	unsigned int active = 0;
	for (unsigned int i = 0; i < m_AIEntities.size(); ++i)
	{
		if (m_AIEntities[i]->isEnabled())
			++active;
	}

	string message;
	message += "Request Pool in AIModule is not large enough. Adjust the macro at the top of this file. Active Minions == ";
	message += to_string(active);
	ASSERT(m_freeRequests.size() > 0, message.c_str());
#endif
#pragma endregion

	/// Don't process if agent is already being processed
	unsigned int size = (unsigned int)m_processing.size();
	for (unsigned int i = 0; i < size; ++i)
	{
		if (m_processing[i]->agent == _agent)
			return;
	}

	/// Remove a Request from the 'free' list of requests
	Request* temp = m_freeRequests.back();
	m_freeRequests.pop_back();

	/// Fill out the Request with the parameters
	temp->agent   = _agent;
	temp->start   = _agent->transform->GetPosition();
	temp->end     = _agent->GetDestination();
	_agent->GetPath().clear();
	temp->path    = &_agent->GetPath();
	temp->navMesh = &m_navMesh;

	/// If a request has already been made by this agent, remove the old request
	size = (unsigned int)m_requests.size();
	for (unsigned int i = 0; i < size; ++i)
	{
		if (m_requests[i]->agent == _agent)
		{
			iter_swap(m_requests.begin() + i, m_requests.end() - 1);
			m_requests.erase(m_requests.end() - 1);
			break;
		}
	}

	/// Push request into the queue
	m_requests.push_back(temp);
}

void AIModule::EnterCutscene()
{
	BehaviorTree* tree = nullptr;

	unsigned int size = (unsigned int)m_AIEntities.size();
	for (unsigned int i = 0; i < size; ++i)
	{
		BossMinion* boss = m_AIEntities[i]->gameObject->GetComponent<BossMinion>();
		if ((m_AIEntities[i]->isEnabled() && m_AIEntities[i]->gameObject->ActiveInHierarchy()) || boss != nullptr)
		{
			tree = m_AIEntities[i]->gameObject->GetComponent<BehaviorTree>();
			if ((tree != nullptr && tree->isEnabled()) || boss != nullptr)
			{
				tree->SwitchToCutsceneTree();
			}
		}
	}
}

void AIModule::ExitCutscene()
{
	BehaviorTree* tree = nullptr;

	unsigned int size = (unsigned int)m_AIEntities.size();
	for (unsigned int i = 0; i < size; ++i)
	{
		if (m_AIEntities[i]->isEnabled() && m_AIEntities[i]->gameObject->ActiveInHierarchy())
		{
			tree = m_AIEntities[i]->gameObject->GetComponent<BehaviorTree>();
			if (tree != nullptr && tree->isEnabled())
			{
				tree->SwitchToNormalTree();
			}
		}
	}
}

