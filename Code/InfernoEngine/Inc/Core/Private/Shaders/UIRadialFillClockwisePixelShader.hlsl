#define IE_DEGTORAD 0.017453292f
#define IE_RADTODEG 57.295779513f

Texture2D Image : register(t0);
SamplerState filter : register(s0);

struct PSInput
{
	float2 uv : TEXTCOORD;
	float2 position : TEXTCOORD2;
};

cbuffer Color : register (b0)
{
	float4 color;
}
cbuffer FillRatio : register(b1)
{
	float angleRatio;
	float3 RadialPad;
}
cbuffer UVOffsets : register(b3)
{
	float2 offset;
	float2 tiling;
}

float4 main(PSInput input) : SV_TARGET
{
	float4 diffuse = Image.Sample(filter, (input.uv * tiling) + offset);

	float2 pixPos = normalize(input.position);
	float pixAngle = (atan2(pixPos.y, pixPos.x) * IE_RADTODEG) + 180;

	if (pixAngle < 360.0f - (360.0f * angleRatio))
		diffuse.a = 0.0f;

	return diffuse * color;
}