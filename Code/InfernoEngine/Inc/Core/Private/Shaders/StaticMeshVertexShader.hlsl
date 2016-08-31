struct VS_IN
{
	float4 Pos : POSITION;
	float3 Norm : NORMAL;
	float3 Tangent : TANGENT;
	float2 uv : TEXTCOORD;
};

struct VS_OUT
{
	float2 uv : TEXTCOORD;
	float4 Pos : SV_POSITION;
	float3 wPos : POSITION;
	float3 Norm : NORMAL;
	float3 Tangent : TANGENT;
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
	output.wPos = output.Pos.xyz;
	output.Pos = mul(ViewMatrix, output.Pos);
	output.Pos = mul(ProjectionMatrix, output.Pos);

	output.Norm = mul((float3x3)WorldMatrix, input.Norm);
	output.Tangent = mul((float3x3)WorldMatrix, input.Tangent);
	output.uv = input.uv;

	return output;
}