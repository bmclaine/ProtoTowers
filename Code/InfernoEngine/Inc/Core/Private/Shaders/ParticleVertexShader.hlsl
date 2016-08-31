struct ParticlePartA
{
	float3 Position;
	float  Lifetime;

	float3 Velocity;
	float  Age;

	float4 Color;
};

StructuredBuffer<ParticlePartA> ParticleABuffer : register(t0);
StructuredBuffer<float4> ViewSpacePositions : register(t1);
StructuredBuffer<float2> IndexList : register(t2);

cbuffer ActiveList : register(b0)
{
	uint AliveCount;
	uint3 AlivePadding;
}

struct VS_OUT
{
	float4 ViewSpaceAndRadius : TEXTCOORD0;
	float4 Color : COLOR;
};

VS_OUT main(uint VertexId : SV_VertexID)
{
	VS_OUT output = (VS_OUT)0;

	uint index = (uint)IndexList[AliveCount - VertexId - 1].y;

	ParticlePartA partA = ParticleABuffer[index];

	output.ViewSpaceAndRadius = ViewSpacePositions[index];
	output.Color = partA.Color;

	return output;
}