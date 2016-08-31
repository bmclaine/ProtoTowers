///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: A NavMesh will have a container of NavNodes to represent a navigation area
//
///////////////////////////////////////////////////////////////////////////////////////////////////


// STL Includes
using namespace std;


// Engine Includes
#include "AI\NavMesh.h"
#include "Assets\Mesh.h"
#include "AI\Triangle.h"
#include "AI\NavNode.h"
#include "Components\NavAgent.h"
#include "Components\Transform.h"


NavMesh::NavMesh()
	: m_bCleanNavMesh(true)
	, m_startNode(nullptr)
	, m_goalNode(nullptr)
	, m_stName(string("UnnamedNavMesh"))
{ }

NavMesh::~NavMesh()
{
	Clear();
}

void NavMesh::Clear()
{
	for (unsigned int i = 0; i < m_navMesh.size(); ++i)
		delete m_navMesh[i];
	m_navMesh.clear();
}

// IN: _mesh - The mesh to load from
//
// OUT: bool - Whether it loaded or not
//
// Description: Loads a mesh into a container, representing a NavMesh
bool NavMesh::Load(Mesh* _mesh, Matrix4& _globalPosition)
{
	// Dat safety check doe
	ASSERT(_mesh->GetSubMeshes()[0].vertices.size() != 0, "NavMesh::Load() -> Mesh parameter doesn't have any vertices in it. Note that the object MUST be named 'NavMesh'.");

	// Delete any navMesh that was already around
	Clear();

	m_stName = _mesh->m_Name;

	// Load in the new NavMesh
	vector<Vertex>       vertices = _mesh->GetSubMeshes()[0].vertices;
	vector<unsigned int> indices  = _mesh->GetSubMeshes()[0].indices;
	for (unsigned int i = 0; i < indices.size();)
	{
		Triangle* tri = new Triangle
			(
				vertices[indices[i++]].position,
				vertices[indices[i++]].position,
				vertices[indices[i++]].position
			);
		NavNode* node = new NavNode(tri);
		m_navMesh.push_back(node);
	}

	// Find each node's neighbors - This method is stupidly slow, but it works
	for (unsigned int i = 0; i < m_navMesh.size(); ++i)
	{
		for (unsigned int j = 0; j < m_navMesh.size(); ++j)
		{
			if (i != j)
				m_navMesh[i]->AddNeighbor(m_navMesh[j]);
		}
	}

	ASSERT(m_navMesh.size() != 0, "NavMesh::Load() -> NavMesh size is 0");
	return true;
}

// IN: VOID
//
// OUT: NavNode* - The starting node of the agent
//
// Descrition: Returns the starting node of the agent
NavNode* NavMesh::GetStartNode()
{
	return m_startNode;
}

// IN: VOID
//
// OUT: NavNode* - The starting node of the agent
//
// Descrition: Returns the starting node of the agent
NavNode* NavMesh::GetGoalNode()
{
	return m_goalNode;
}

// IN: VOID
//
// OUT: bool - The classes flag member
//
// Description: Returns true if the NavMesh is clean, false if the NavMesh is dirty
bool NavMesh::GetCleanFlag() const
{
	return m_bCleanNavMesh;
}

// IN: VOID
//
// OUT: VOID
//
// Description: This will update the NavMesh to account for all obstacles if the flag is set to dirty
void NavMesh::Update()
{
	if (m_bCleanNavMesh == false)
	{
		Clean();
		m_bCleanNavMesh = true;
	}
}

// IN: VOID
//
// OUT: NavMesh* - An exact copy of the invoking object
//
// Description: The invoking object makes a copy of itself then returns a pointer to the new data
NavMesh* NavMesh::Copy() const
{
	return new NavMesh(*this);
}

// IN: The agent to calculate costs with
//
// OUT: VOID
//
// Description: All costs of the navMesh are calculated using the _agent as an influence
void NavMesh::InitializeTileCosts(NavAgent* _agent)
{
	float closestTileToAgent = FLT_MAX;
	float closestTileToGoal  = FLT_MAX;
	float distanceToAgent    = FLT_MAX;
	float distanceToGoal     = FLT_MAX;

	// Find the closest tile to the player to treat as the startNode & set the endNode
	for (unsigned int i = 0; i < m_navMesh.size(); ++i)
	{
		Vector3 centroid = m_navMesh[i]->GetCentroid();
		distanceToGoal = (centroid - _agent->GetDestination()).MagnitudeSq();
		if (distanceToGoal < closestTileToGoal)
		{
			closestTileToGoal = distanceToGoal;
			m_goalNode = m_navMesh[i];
		}
		distanceToAgent = (centroid - _agent->transform->GetPosition()).MagnitudeSq();
		if (distanceToAgent < closestTileToAgent)
		{
			closestTileToAgent = distanceToAgent;
			m_startNode = m_navMesh[i];
		}
	}

	m_goalNode->SetGivenCost(0);
	m_goalNode->SetHeuristicCost(0);
	m_startNode->SetGivenCost(0);
	m_startNode->SetHeuristicCost((m_goalNode->GetCentroid() - m_startNode->GetCentroid()).MagnitudeSq());

	// Calculate all the weights for the nodes
	for (unsigned int i = 0; i < m_navMesh.size(); ++i)
		m_navMesh[i]->SetHeuristicCost((m_goalNode->GetCentroid() - m_navMesh[i]->GetCentroid()).MagnitudeSq());
}

// IN: VOID
//
// OUT: VOID
//
// Description: Resets each NavNode in the mesh to their default values
void NavMesh::Reset()
{
	unsigned int size = (unsigned int)m_navMesh.size();
	for (unsigned int i = 0; i < size; ++i)
	{
		m_navMesh[i]->SetGivenCost(0);
		m_navMesh[i]->SetHeuristicCost(0);
		m_navMesh[i]->SetParent(nullptr);
		m_navMesh[i]->SetVisited(false);
	}
}

void NavMesh::Clean()
{
	for (unsigned int i = 0; i < m_navMesh.size(); ++i)
	{
		// TODO: Mark each tile as "not walkable" if an abstacle is in the way
	}
}

const std::string& NavMesh::GetName() const
{
	return m_stName;
}
std::string& NavMesh::GetName()
{
	return m_stName;
}

