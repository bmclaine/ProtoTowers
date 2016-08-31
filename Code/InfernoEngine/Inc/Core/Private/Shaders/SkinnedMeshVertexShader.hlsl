struct VS_IN
{
	float4 Pos : POSITION;
	float3 Norm : NORMAL;
	float3 Tangent : TANGENT;
	float2 uv : TEXTCOORD;
	float4 Influences : BONEINFLUENCES;
	float4 Weights : BONEWEIGHTS;
};

struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float3 Norm : NORMAL;
	float3 Tangent : TANGENT;
	float2 uv : TEXTCOORD;
	float4 Influences : BONEINFLUENCES;
	float4 Weights : BONEWEIGHTS;
};

cbuffer View : register (b1)
{
	float4x4 ViewMatrix;
}
cbuffer Projection : register (b2)
{
	float4x4 ProjectionMatrix;
}
cbuffer Bones : register (b3)
{
	float4x4 BoneMatrices[100];
}

VS_OUT main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	float4x4 WorldMatrix = input.Weights.x * BoneMatrices[input.Influences.x];
	WorldMatrix += input.Weights.y * BoneMatrices[input.Influences.y];
	WorldMatrix += input.Weights.z * BoneMatrices[input.Influences.z];
	WorldMatrix += input.Weights.w * BoneMatrices[input.Influences.w];

	output.Pos = mul(WorldMatrix, input.Pos);
	//output.Pos = mul(ViewMatrix, output.Pos);
	//output.Pos = mul(ProjectionMatrix, output.Pos);

	output.Norm = mul((float3x3)WorldMatrix, input.Norm);
	output.Tangent = mul((float3x3)WorldMatrix, input.Tangent);
	output.uv = input.uv;

	return output;
}