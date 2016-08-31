///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: This class will keep track of each triangle in a NavMesh. It is the equivelent to
//              a "tile" for our Pathfinding lab in our AI class.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once


// STL Includes
#include <vector>


// Engine Includes
#include "MathLib.h"



class Triangle
{
private:
	Vector3 m_positions[3];  // -> The positions of each of the Triangle's vertices
	int     m_weight;        // -> The weight of the tile

public:
	// Defaults
	Triangle ( Vector3 _vert1 = Vector3::Zero(), Vector3 _vert2 = Vector3::Zero(), Vector3 _vert3 = Vector3::Zero(), int _weight = 0 );
	~Triangle( void );




	// IN: _index - The index of the vertex
	//
	// OUT: Vector3 - The position of the vertex
	//
	// Description: Returns the position of the vertex at the specified index
	const Vector3& GetPosition( int _index ) const;



	
	// IN: VOID
	//
	// OUT: int - The weight of the tile
	//
	// Description: Returns the weight of the tile
	int GetWeight( void ) const;




	// IN: _index - The index of in the position array to set
	//     _position - The Vector3 representing position
	//
	// OUT: VOID
	//
	// Description: Sets the first position index of the triangle to the parameter
	void SetPosition(int _index, Vector3 _position);




	// IN: _weight - The new weight for the tile
	//
	// OUT: VOID
	//
	// Description: Sets the tile's weight to the parameter
	void SetWeight(int _weight);
};

