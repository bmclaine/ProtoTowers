// Author : Jack Gillette
//
// Fills in an Image from Bottom to Top based from a passed in Ratio

Texture2D image : register(t0);
SamplerState filter : register(s0);

cbuffer Color : register (b0)
{
	float4 color;
}
cbuffer FillRatio : register (b1)
{
	float fillRatio;
	float3 FillPad;
}
cbuffer UVOffsets : register(b3)
{
	float2 offset;
	float2 tiling;
}

float4 main(float2 uv : TEXTCOORD) : SV_TARGET
{
	float ratio = 1 - fillRatio;
	float alpha = float(uv.y > ratio);
	float4 sampledColor = image.Sample(filter, (uv * tiling) + offset) * color;
	
	return float4(sampledColor.x, sampledColor.y, sampledColor.z, sampledColor.w * alpha);
}