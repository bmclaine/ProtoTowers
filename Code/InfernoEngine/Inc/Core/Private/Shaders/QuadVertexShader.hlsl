struct VS_IN
{
	float4 Pos : POSITION;
	float3 Norm : NORMAL;
	float2 uv : TEXTCOORD;
};

struct VS_OUT
{
	float3 Norm : NORMAL;
	float2 uv : TEXTCOORD;
	float4 Pos : SV_POSITION;
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

	output.Pos = mul(WorldMatrix, input.Pos);
	output.Pos = mul(ViewMatrix, output.Pos);
	output.Pos = mul(ProjectionMatrix, output.Pos);

	output.Norm = mul(WorldMatrix, float4(input.Norm, 0)).xyz;
	output.uv = input.uv;

	return output;
}