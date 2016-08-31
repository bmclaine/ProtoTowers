Texture2D image : register(t0);
Texture2D image2 : register(t1);
SamplerState filter : register(s0);

cbuffer Color : register (b0)
{
	float4 color;
}
cbuffer Color2 : register (b1)
{
	float4 color2;
}
cbuffer FillRatio : register (b2)
{
	float fillRatio;
	float3 FillPad;
}

float4 main(float2 uv : TEXTCOORD) : SV_TARGET
{
	float alpha = float(uv.x < fillRatio);
	float alpha2 = 1.0f - alpha;

	float4 sampledColor = image.Sample(filter, uv) * color;
	float4 sampledColor2 = image2.Sample(filter, uv) * color2;

	float4 finalColor = float4(sampledColor.x * alpha, sampledColor.y * alpha, sampledColor.z * alpha, sampledColor.w * alpha) +
	float4(sampledColor2.x * alpha2, sampledColor2.y * alpha2, sampledColor2.z * alpha2, sampledColor2.w * alpha2);

	return finalColor;
}