///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: This class is used with the sole purpose of pathfinding for a NavAgent.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once


// STL Includes
#include <functional>
#include <vector>

// Engine Includes
#include "MathLib.h"
#include "AI\NavNode.h"

// Forward Declarations
class NavAgent;
struct Request;

class PathfindingThread
{
private:
	// Custom comparator for the priority_queue
	struct SortByFinalCost
	{
		inline bool operator()(const NavNode* _left, const NavNode* _right) const
		{
			return _left->GetFinalCost() > _right->GetFinalCost();
		}
	};

	std::vector<NavNode*> m_openList;
	Request* m_pRequest;     // -> A pointer to the request that was made by the AIModule
	NavNode* m_currentNode;  // -> The currentNode to be processing
	NavNode* m_neighborNode; // -> The neighborNode to the currentNode

	bool Loop( void );
	void CalculateAgentPath( void );
	void ComputeCost(NavNode* _current, NavNode* _neighbor);
	void UpdateVertex(NavNode* _current, NavNode* _neighbor);
	bool CanSeeParent(NavNode* _current, NavNode* _neighbor);

public:
	// Defaults
	 PathfindingThread( void );
	~PathfindingThread( void );

	// Overridden classes from parent
	void FindPath ( void );

	// IN: VOID
	//
	// OUT: NavAgent* - The NavAgent in the Request
	//
	// Description: Returns the NavAgent member of the Request class member this is keeping track of
	Request* GetRequest( void ) const;
	Request* GetRequest( void );

	// IN: Request* - The Request to keep track of
	//
	// OUT: VOID
	//
	// Description: Sets the class member to this Request
	void Initialize(Request* _request);
};

