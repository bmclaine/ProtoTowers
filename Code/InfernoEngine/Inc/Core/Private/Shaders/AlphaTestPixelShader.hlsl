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

void main(float2 uv : TEXTCOORD)// : SV_Coverage
{
	float alpha = image.Sample(filter, (uv * tiling) + offset).a * color.a;

	if (alpha < 0.8f)
		discard;
}