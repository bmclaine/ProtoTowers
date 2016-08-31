///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: A NavMesh will have a container of NavNodes to represent a navigation area
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

// STL Includes
#include <vector>

// Engine Includes
#include "MathLib.h"

// Forward Declarations
class Mesh;
class NavNode;
class NavAgent;

class NavMesh
{
private:
	std::vector < NavNode* > m_navMesh;      // -> Le NavMeshe
	bool                     m_bCleanNavMesh;// -> A flag for a clean or dirty NavMesh
	NavNode*                 m_startNode;
	NavNode*                 m_goalNode;
	std::string              m_stName;

	// Private function to clean the navMesh if dirty
	void Clean( void );
	void Clear( void );

public:
	// Defaults
	NavMesh ( void );
	~NavMesh( void );

	// IN: _mesh - The mesh to load from
	//
	// OUT: bool - Whether it loaded or not
	//
	// Description: Loads a mesh into a container, representing a NavMesh
	bool Load(Mesh* _mesh, Matrix4& _globalPosition);

	// IN: VOID
	//
	// OUT: NavNode* - The starting node of the agent
	//
	// Descrition: Returns the starting node of the agent
	NavNode* GetStartNode( void );

	// IN: VOID
	//
	// OUT: NavNode* - The starting node of the agent
	//
	// Descrition: Returns the starting node of the agent
	NavNode* GetGoalNode(void);

	// IN: VOID
	//
	// OUT: bool - The classes flag member
	//
	// Description: Returns true if the NavMesh is clean, false if the NavMesh is dirty
	bool GetCleanFlag( void ) const;

	// IN: VOID
	//
	// OUT: VOID
	//
	// Description: This will update the NavMesh to account for all obstacles if the flag is set to dirty
	void Update( void );

	// IN: VOID
	//
	// OUT: NavMesh* - An exact copy of the invoking object
	//
	// Description: The invoking object makes a copy of itself then returns a pointer to the new data
	NavMesh* Copy(void) const;

	// IN: The agent to calculate costs with
	//
	// OUT: VOID
	//
	// Description: All costs of the navMesh are calculated using the _agent as an influence
	void InitializeTileCosts( NavAgent* _agent );

	// IN: VOID
	//
	// OUT: VOID
	//
	// Description: Resets each NavNode in the mesh to their default values
	void Reset( void );

	const std::string& GetName( void ) const;
	std::string& GetName( void );
};


