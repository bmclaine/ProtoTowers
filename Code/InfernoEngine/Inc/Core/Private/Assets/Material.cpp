#include "Assets\Material.h"

Material::Material()
{
	DiffuseMapID = -1;
	NormalMapID = -1;
	OcclusionMapID = -1;

	uvOffsets = Vector2(0, 0);
	uvTiling = Vector2(1, 1);

	EffectID = -1;
}

Vector2 Material::GetOffsets()
{
	return uvOffsets;
}
Vector2 Material::GetTiling()
{
	return uvTiling;
}

void Material::SetOffsets(Vector2 offsets)
{
	offsets.x = fmodf(offsets.x, 1.0f);
	offsets.y = fmodf(offsets.y, 1.0f);

	if (offsets.x < 0)
		offsets.x += 1.0f;
	if (offsets.y < 0)
		offsets.y += 1.0f;

	uvOffsets = offsets;
}
void Material::SetTiling(Vector2 tiling)
{
	uvTiling = tiling;
}