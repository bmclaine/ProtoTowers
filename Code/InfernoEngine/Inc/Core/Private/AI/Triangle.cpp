///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: This class will keep track of each triangle in a NavMesh. It is the equivelent to
//              a "tile" in our Pathfinding lab for our AI class.
//
///////////////////////////////////////////////////////////////////////////////////////////////////


// Engine Includes
#include "AI\Triangle.h"


// STL Includes
using namespace std;


Triangle::Triangle(Vector3 _vert1, Vector3 _vert2, Vector3 _vert3, int _weight)
{
	m_positions[ 0 ]   = _vert1;
	m_positions[ 1 ]   = _vert2;
	m_positions[ 2 ]   = _vert3;

	m_weight = 1;
}



Triangle::~Triangle()
{
	
}




// IN: _index - The index of the vertex
//
// OUT: Vector3 - The position of the vertex
//
// Description: Returns the position of the vertex at the specified index
const Vector3& Triangle::GetPosition(int _index) const
{
	return m_positions[_index];
}




// IN: VOID
//
// OUT: int - The weight of the tile
//
// Description: Returns the weight of the tile
int Triangle::GetWeight() const
{
	return m_weight;
}




// IN: _index - The index of in the position array to set
//     _position - The Vector3 representing position
//
// OUT: VOID
//
// Description: Sets the first position index of the triangle to the parameter
void Triangle::SetPosition(int _index, Vector3 _position)
{
	m_positions[_index] = _position;
}




// IN: _weight - The new weight for the tile
//
// OUT: VOID
//
// Description: Sets the tile's weight to the parameter
void Triangle::SetWeight(int _weight)
{
	m_weight = _weight;
}




