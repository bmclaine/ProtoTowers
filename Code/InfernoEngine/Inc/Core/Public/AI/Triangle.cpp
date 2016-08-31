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
#include <list>


// Engine Includes
#include "MathLib.h"



class Triangle
{
private:
	Vector3                 m_position[3];
	std::list < Triangle* > m_neighbors;

public:
	// Defaults
	Triangle ( void );
	~Triangle( void );
};

