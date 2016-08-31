//*********************************************************************//
//	Author:			Brian McLaine
//	Description:	
//*********************************************************************//
#pragma once

#include "MathLib.h"

#include <vector>

#define MIN_BONE_INFLUENCES 4

struct BoneInfluence
{
	float boneIndex;
	float boneWeight;
};

struct LoadedVertex;

struct Vertex
{
	Vector4 position;
	Vector3 normal;
	Vector3 tangent;
	Vector2 uv;

	Vector4 boneIndices;
	Vector4 boneWeights;

	bool Vertex::operator()(const Vertex& lhs, const Vertex& rhs) const;
	bool Vertex::operator==(const Vertex& rhs) const;
	bool Vertex::operator<(const Vertex& rhs) const;

	void Vertex::operator=(const LoadedVertex& rhs);
};

struct LoadedVertex : Vertex
{
	unsigned int controlPointIndex;

	bool LoadedVertex::operator()(const LoadedVertex& lhs, const LoadedVertex& rhs) const;
	bool LoadedVertex::operator==(const LoadedVertex& rhs) const;
	bool LoadedVertex::operator<(const LoadedVertex& rhs) const;
};

namespace std
{
	template <>
	struct hash<LoadedVertex>
	{
		std::size_t operator()(const LoadedVertex& k) const
		{
			return ((hash<float>()(k.position.x)
				^ (hash<float>()(k.position.y) << 1)) >> 1)
				^ (hash<float>()(k.position.z) << 1);
		}
	};

	template <>
	struct hash<Vertex>
	{
		std::size_t operator()(const Vertex& k) const
		{
			return ((hash<float>()(k.position.x)
				^ (hash<float>()(k.position.y) << 1)) >> 1)
				^ (hash<float>()(k.position.z) << 1);
		}
	};
}