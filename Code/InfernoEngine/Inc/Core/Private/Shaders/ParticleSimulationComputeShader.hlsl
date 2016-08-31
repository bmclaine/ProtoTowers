struct ParticlePartA
{
	float3 Position;
	uint   Flags;

	float3 Velocity;
	float  Age;

	float4 Color;
};
struct ParticlePartB
{
	float4 StartColor;
	float4 EndColor;

	float  Lifetime;
	float  DistanceToEye;
	uint   IsSleeping;
	float  GravityScale;

	float  StartSize;
	float  EndSize;
	float2 PartBPad;
};

RWStructuredBuffer<ParticlePartA> ParticleABuffer : register(u0);
RWStructuredBuffer<ParticlePartB> ParticleBBuffer : register(u1);

AppendStructuredBuffer<uint> DeadList : register(u2);

RWStructuredBuffer<float2> IndexList : register(u3);
RWStructuredBuffer<float4> ViewSpacePositions : register(u4);
RWBuffer<uint> DrawArgs : register(u5);

Texture2D DepthBuffer : register(t0);

cbuffer View : register(b0)
{
	float4x4 ViewMatrix;
}
cbuffer FrameInformation : register(b1)
{
	float4 CameraPostion;
	float DeltaTime;
	uint ScreenWidth;
	uint ScreenHeight;
	float pad;
}

[numthreads(256, 1, 1)]
void main(uint3 ThreadID : SV_DispatchThreadID)
{
	/*if (ThreadID.x == 0)
	{
		DrawArgs[0] = 0;
		DrawArgs[1] = 1;
		DrawArgs[2] = 0;
		DrawArgs[3] = 0;
		DrawArgs[4] = 0;
	}

	GroupMemoryBarrierWithGroupSync();*/

	const float3 Gravity = float3(0.0, -20.0f, 0.0);

	ParticlePartA partA = ParticleABuffer[ThreadID.x];
	ParticlePartB partB = ParticleBBuffer[ThreadID.x];

	if (partA.Age > 0.0f)
	{
		//float3 viewPosition = mul(ViewMatrix, float4(partA.Position, 1)).xyz;

		partA.Age -= DeltaTime;

		if (partB.IsSleeping == 0)
		{
			partA.Velocity += Gravity * partB.GravityScale * DeltaTime;
			partA.Position += partA.Velocity * DeltaTime;
		}

		float3 ToCamera = partA.Position - CameraPostion.xyz;
		partB.DistanceToEye = length(ToCamera);

		float ScaledLife = 1.0f - saturate(partA.Age / partB.Lifetime);
		partA.Color = lerp(partB.StartColor, partB.EndColor, ScaledLife);

		float currentRadius = lerp(partB.StartSize, partB.EndSize, ScaledLife);
		ViewSpacePositions[ThreadID.x] = float4(mul(ViewMatrix, float4(partA.Position, 1)).xyz, currentRadius);

		if (partA.Age <= 0.0f)
		{
			partA.Age = -1.0f;
			DeadList.Append(ThreadID.x);
		}
		else
		{
			uint Index = IndexList.IncrementCounter();
			IndexList[Index] = float2(partB.DistanceToEye, (float)ThreadID.x);

			/*uint ID = 0;
			InterlockedAdd(DrawArgs[0], 1, ID);
			partB.PartBPad[0] = asfloat(ID);*/
		}
	}

	ParticleABuffer[ThreadID.x] = partA;
	ParticleBBuffer[ThreadID.x] = partB;
}