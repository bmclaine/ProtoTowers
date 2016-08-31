struct VS_IN
{
	float4 pos : POSITION;
	float2 uv : TEXTCOORD;
};
struct VS_OUT
{
	float2 uv : TEXTCOORD;
	float4 pos : SV_POSITION;
};

cbuffer World : register (b0)
{
	float4x4 WorldMatrix;
}
cbuffer View : register (b1)
{
	float4x4 ViewMatrix;
}
cbuffer Projection : register (b2)
{
	float4x4 ProjectionMatrix;
}

VS_OUT main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	output.pos = mul(WorldMatrix, input.pos);
	output.pos = mul(ViewMatrix, output.pos);
	output.pos = mul(ProjectionMatrix, output.pos);
	output.uv = input.uv;

	return output;
}