//*********************************************************************//
//	Author:			Brian McLaine
//	Description:	
//*********************************************************************//

#include "Assets\Vertex.h"

bool Vertex::operator()(const Vertex& lhs, const Vertex& rhs) const
{
	return (lhs.position.x == rhs.position.x &&
		lhs.position.y == rhs.position.y &&
		lhs.position.z == rhs.position.z &&
		lhs.position.w == rhs.position.w &&
		lhs.normal.x == rhs.normal.x &&
		lhs.normal.y == rhs.normal.y &&
		lhs.normal.z == rhs.normal.z &&
		lhs.tangent.x == rhs.tangent.x &&
		lhs.tangent.y == rhs.tangent.y &&
		lhs.tangent.z == rhs.tangent.z &&
		lhs.uv.x == rhs.uv.x &&
		lhs.uv.y == rhs.uv.y);
}

bool Vertex::operator==(const Vertex& rhs) const
{
	return operator()(*this, rhs);
}

bool Vertex::operator<(const Vertex& rhs) const
{
	return std::hash<Vertex>().operator()(*this) < std::hash<Vertex>().operator()(rhs);
}

void Vertex::operator=(const LoadedVertex& rhs)
{
	this->position = rhs.position;
	this->normal = rhs.normal;
	this->tangent = rhs.tangent;
	this->uv = rhs.uv;
	this->boneIndices = rhs.boneIndices;
	this->boneWeights = rhs.boneWeights;
}

bool LoadedVertex::operator()(const LoadedVertex& lhs, const LoadedVertex& rhs) const
{
	return (lhs.position.x == rhs.position.x &&
		lhs.position.y == rhs.position.y &&
		lhs.position.z == rhs.position.z &&
		lhs.position.w == rhs.position.w &&
		lhs.normal.x == rhs.normal.x &&
		lhs.normal.y == rhs.normal.y &&
		lhs.normal.z == rhs.normal.z &&
		lhs.tangent.x == rhs.tangent.x &&
		lhs.tangent.y == rhs.tangent.y &&
		lhs.tangent.z == rhs.tangent.z &&
		lhs.uv.x == rhs.uv.x &&
		lhs.uv.y == rhs.uv.y &&
		lhs.controlPointIndex == rhs.controlPointIndex);
}

bool LoadedVertex::operator==(const LoadedVertex& rhs) const
{
	return operator()(*this, rhs);
}

bool LoadedVertex::operator<(const LoadedVertex& rhs) const
{
	return std::hash<LoadedVertex>().operator()(*this) < std::hash<LoadedVertex>().operator()(rhs);
}