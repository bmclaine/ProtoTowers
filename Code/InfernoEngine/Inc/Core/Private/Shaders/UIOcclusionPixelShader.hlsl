Texture2D<float4> DiffuseMap : register(t0);
Texture2D<float4> OcclusionMap : register(t2);

SamplerState filter : register(s0);

cbuffer Color : register(b0)
{
	float4 color;
}
cbuffer UVOffsets : register(b3)
{
	float2 offset;
	float2 tiling;
}

float4 main(float2 uv : TEXTCOORD) : SV_TARGET
{
	float4 Diffuse = DiffuseMap.Sample(filter, uv + offset);
	Diffuse.w = OcclusionMap.Sample(filter, (uv * tiling) + offset).x;

	return Diffuse * color;
}