///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: This class is what the NavMesh will be made of. This node class is similar to
//              the node structure I made in AI class. It will have a Triangle(tile) which
//              will know about its neighbors.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once


// Engine Includes
#include "MathLib.h"
#include "GlobalIncludes.h"


// STL Includes
#include <vector>


// Forward Declarations
class Triangle;
class NavAgent;



class NavNode
{
private:
	Triangle *m_pTile;   // -> The triangle in the NavMesh associated with this node
	NavNode  *m_pParent; // -> The parent Node to this one
	std::vector<NavNode*> m_vNeighbors;// -> Neighbors to this navNode
	Vector3   m_centroid; // -> The centroid of the tile
	
	bool  m_visited;
	
	float m_fGCost; // -> The nodes Given cost
	float m_fHCost; // -> The nodes Heuristic cost

	bool  m_isTraversable;


	// IN: _neighbor* - The triangle to check for neighbor-age
	//
	// OUT: bool - If the two triangle are neighbors
	//
	// Description: Returns true if the passed in triangle is a neighbor to the invoking object.
	//              Returns false if not.
	bool IsAdjacent(NavNode* _neighbor);

	// IN: Triangle* - The triangle
	//
	// OUT: VOID
	//
	// Description: Sets this NavNodes centroid based on the triangles positions
	void SetCentroid(Triangle* _tri);


public:
	// Defaults
	NavNode ( Triangle* _tile = nullptr, NavNode* _parent = nullptr, bool _visited = false, float _gCost = 0, float _hCost = 0, bool _traversable = true );
	~NavNode( void );




	// IN: _neighbor - The neighbor to add
	//
	// OUT: VOID
	//
	// Description: Returns the neighbors to this NavNode
	void AddNeighbor(NavNode* _neighbor);




	// IN: VOID
	//
	// OUT: vector<NavNode*> - The list of neighbors
	//
	// Description: Returns the list of neighbors to this NavNode
	const std::vector<NavNode*> GetNeighbors( void ) const;
	std::vector<NavNode*> GetNeighbors( void );




	// IN: VOID
	//
	// OUT: Triangle* - The tile class member
	//
	// Description: Returns the triangle pointer class member
	Triangle* GetTile ( void ) const;



	// IN: VOID
	//
	// OUT: NavNode* - The parent pointer class member
	//
	// Description: Returns the parent to this node
	NavNode*  GetParent ( void ) const;




	// IN: VOID
	//
	// OUT: Vector3 - The positino of the triangle
	//
	// Description: Returns the average of a triangles three verts
	Vector3 GetCentroid( void ) const;



	// IN: _agent* - The agent to check if it's visited
	//
	// OUT: bool - Returns if the agent has been to this node
	//
	// Description: The pathfinding will be on multiple threads, so to keep track of all of them, we use a container
	bool BeenVisited( void ) const;
	
	
	
	// IN: VOID
	//
	// OUT: float - The given cost of the tile
	//
	// Description: Returns the given cost of this node
	float GetGivenCost ( void ) const;



	// IN: VOID
	//
	// OUT: float - The heuristic cost of the tile
	//
	// Description: Returns the heuristic cost of this node
	float GetHeuristicCost( void ) const;



	// IN: VOID
	//
	// OUT: float - The final cost of the tile
	//
	// Description: Returns the final cost of this node
	float GetFinalCost( void ) const;




	// IN: _tile - The tile to set the class member to
	//
	// OUT: VOID
	//
	// Description: Sets the class member to the parameter
	void SetTile(Triangle* _tile);



	// IN: _parent - The parent to set the class member to
	//
	// OUT: VOID
	//
	// Description: Sets the class member to the parameter
	void SetParent(NavNode* _parent);
	

	
	// IN: _visited - The value to set the member to
	//
	// OUT: VOID
	//
	// Description: Sets the m_visited class member
	void SetVisited(bool _visited);



	// IN: _g - The new given cost
	//
	// OUT: VOID
	//
	// Description: Sets the given cost of the node to the parameter
	void SetGivenCost(float _g);



	// IN: _h - The new heuristic cost
	//
	// OUT: VOID
	//
	// Description: Sets the heuristic cost of the node to the parameter
	void SetHeuristicCost(float _h);
};

