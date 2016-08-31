struct ParticlePartA
{
	float3 Position;
	uint   Flags;

	float3 Velocity;
	float  Age;

	float4 Color;
};

RWStructuredBuffer<ParticlePartA> ParticleABuffer : register(u0);
AppendStructuredBuffer<uint> DeadList : register(u1);

[numthreads(256, 1, 1)]
void main( uint3 ThreadID : SV_DispatchThreadID )
{
	ParticlePartA partA = ParticleABuffer[ThreadID.x];

	if (partA.Age > 0.0f)
	{
		partA.Age = -1.0f;
		DeadList.Append(ThreadID.x);
	}

	ParticleABuffer[ThreadID.x] = partA;
}