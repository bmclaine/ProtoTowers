Texture2D image : register(t0);
SamplerState filter : register(s0);

cbuffer Color : register (b0)
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
	return image.Sample(filter, (uv * tiling) + offset) * color;
}