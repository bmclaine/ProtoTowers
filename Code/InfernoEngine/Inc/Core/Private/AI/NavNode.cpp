///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: This class is what the NavMesh will be made of. This node class is similar to
//              the node structure I made in AI class. It will have a Triangle(tile) which
//              will know about its neighbors.
//
///////////////////////////////////////////////////////////////////////////////////////////////////



// STL Includes


// Engine Includes
#include "AI\NavNode.h"
#include "AI\Triangle.h"





NavNode::NavNode(Triangle* _tile, NavNode* _parent, bool _visited, float _gCost, float _hCost, bool _traversable)
{
	m_pTile         = _tile;
	m_pParent       = _parent;
	m_visited       = _visited;
	m_fGCost        = _gCost;
	m_fHCost        = _hCost;
	m_isTraversable = _traversable;
	
	SetCentroid(m_pTile);
}


NavNode::~NavNode()
{
	if (m_pTile != nullptr)
		delete m_pTile;
	m_pTile = nullptr;
}


// IN: VOID
//
// OUT: Triangle* - The tile class member
//
// Description: Returns the triangle pointer class member
Triangle* NavNode::GetTile() const
{
	return m_pTile;
}



// IN: VOID
//
// OUT: NavNode* - The parent pointer class member
//
// Description: Returns the parent to this node
NavNode* NavNode::GetParent() const
{
	return m_pParent;
}



// IN: VOID
//
// OUT: Vector3 - The positino of the triangle
//
// Description: Returns the average of a triangles three verts
Vector3 NavNode::GetCentroid() const
{
	return m_centroid;
}



// IN: _agent* - The agent to check if it's visited
//
// OUT: bool - Returns if the agent has been to this node
//
// Description: The pathfinding will be on multiple threads, so to keep track of all of them, we use a container
bool NavNode::BeenVisited() const
{
	return m_visited;
}



// IN: VOID
//
// OUT: float - The given cost of the tile
//
// Description: Returns the given cost of this node
float NavNode::GetGivenCost() const
{
	return m_fGCost;
}



// IN: VOID
//
// OUT: float - The heuristic cost of the tile
//
// Description: Returns the heuristic cost of this node
float NavNode::GetHeuristicCost() const
{
	return m_fHCost;
}



// IN: VOID
//
// OUT: float - The final cost of the tile
//
// Description: Returns the final cost of this node
float NavNode::GetFinalCost() const
{
	return m_fGCost + m_fHCost;
}




// IN: Triangle* - The triangle
//
// OUT: VOID
//
// Description: Sets this NavNodes centroid based on the triangles positions
void NavNode::SetCentroid(Triangle* _tri)
{
	m_centroid = (_tri->GetPosition(0) + _tri->GetPosition(1) + _tri->GetPosition(2)) / 3;
}




// IN: _tile - The tile to set the class member to
//
// OUT: VOID
//
// Description: Sets the class member to the parameter
void NavNode::SetTile(Triangle* _tile)
{
	m_pTile = _tile;
}



// IN: _parent - The parent to set the class member to
//
// OUT: VOID
//
// Description: Sets the class member to the parameter
void NavNode::SetParent(NavNode* _parent)
{
	m_pParent = _parent;
}



// IN: _visited - The value to set the member to
//
// OUT: VOID
//
// Description: Sets the m_visited class member
void NavNode::SetVisited(bool _visited)
{
	m_visited = _visited;
}



// IN: _g - The new given cost
//
// OUT: VOID
//
// Description: Sets the given cost of the node to the parameter
void NavNode::SetGivenCost(float _g)
{
	m_fGCost = _g;
}



// IN: _h - The new heuristic cost
//
// OUT: VOID
//
// Description: Sets the heuristic cost of the node to the parameter
void NavNode::SetHeuristicCost(float _h)
{
	m_fHCost = _h;
}





// IN: _neighbor* - The triangle to check for neighbor-age
//
// OUT: bool - If the two triangle are neighbors
//
// Description: Returns true if the passed in triangle is a neighbor to the invoking object.
//              Returns false if not.
bool NavNode::IsAdjacent(NavNode* _neighbor)
{
	int vertCount = 0;
	for (unsigned int i = 0; i < 3; ++i)
	{
		for (unsigned int j = 0; j < 3; ++j)
		{
			if (GetTile()->GetPosition(i) == _neighbor->GetTile()->GetPosition(j))
				++vertCount;
			if (vertCount == 2)
				return true;
		}
	}
	return false;
}




// IN: _neighbor - The neighbor to add
//
// OUT: VOID
//
// Description: Returns the neighbors to this NavNode
void NavNode::AddNeighbor(NavNode* _neighbor)
{
	ASSERT(_neighbor != nullptr, "Tried to add a nullptr neighbor");
	ASSERT(this != _neighbor, "A node tried to add itself as a neighbor");

	if(IsAdjacent(_neighbor))
		m_vNeighbors.push_back(_neighbor);
}




// IN: _neighbor - The neighbor to add
//
// OUT: VOID
//
// Description: Returns the neighbors to this NavNode
const std::vector<NavNode*> NavNode::GetNeighbors() const
{
	return m_vNeighbors;
}
std::vector<NavNode*> NavNode::GetNeighbors()
{
	return m_vNeighbors;
}




