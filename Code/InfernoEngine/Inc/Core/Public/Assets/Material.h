// STRAWMAN
#pragma once

#include "MathLib.h"

struct Material
{
private:
	Vector2 uvOffsets;
	Vector2 uvTiling;

public:
	unsigned int DiffuseMapID;
	unsigned int NormalMapID;
	unsigned int OcclusionMapID;

	unsigned int EffectID;

	Material();

	Vector2 GetOffsets();
	Vector2 GetTiling();

	void SetOffsets(Vector2 offsets);
	void SetTiling(Vector2 tiling);
};