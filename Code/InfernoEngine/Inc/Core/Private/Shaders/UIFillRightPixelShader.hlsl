// Author : Jack Gillette
//
// Fills in an Image from Right to left based on a passed in Ratio

Texture2D image : register(t0);
SamplerState filter : register(s0);

cbuffer Color : register (b0)
{
	float4 color;
}
cbuffer FillRatio : register (b1)
{
	float fillRatio;
	float3 fillPad;
}
cbuffer UVOffsets : register(b3)
{
	float2 offset;
	float2 tiling;
}

float4 main(float2 uv : TEXTCOORD) : SV_TARGET
{
	float alpha = float(uv.x > 1.0 - fillRatio);
	float4 sampledColor = image.Sample(filter, uv + offset) * color;
	return float4(sampledColor.x, sampledColor.y, sampledColor.z, sampledColor.w * alpha);
}