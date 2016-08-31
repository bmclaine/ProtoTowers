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
ConsumeStructuredBuffer<uint> DeadList : register(u2);

Texture2D RandomValuesTexture : register(t0);
SamplerState Sampler : register (s0);

cbuffer EmitterSettings : register (b0)
{
	float3 WorldPosition;		// Position in world space
	float  Duration;			// Play time of the emitter
	float  AliveTime;			// Time the emitter has been alive
	float  Lifetime;			// The total time a particle is alive
	float  StartSpeed;			// Speed of the particle when spawned
	float  GravityScale;		// Scalar to use for gravity
	float4 StartColor;			// Color of the particle when spawned
	float4 EndColor;			// Color of the particle at death
	float  StartSize;			// Size of the particle when spawned
	float  EndSize;				// Size of the particle at death
	uint   MaxParticles;		// Holds the total number of particles the emitter can have
	uint   Emission;			// How many particles per frame can spawn
	float3 ShapeDimensions;		// Dimensions for the shape
	uint   Flags;				// Flags to be used by the emitter and particles
};

cbuffer EmitterRotation : register (b1)
{
	float4x4 Rotation;
}

cbuffer DeadListCount : register (b2)
{
	uint DeadCount;
	uint3 DeadcountPad;
}

[numthreads(256, 1, 1)]
void main(uint3 ThreadID : SV_DispatchThreadID)
{
	if (ThreadID.x < DeadCount && ThreadID.x < Emission)
	{
		if (Lifetime <= 0)
			return;

		ParticlePartA PartA = (ParticlePartA)0;
		ParticlePartB PartB = (ParticlePartB)0;

		// Create a UV based on our thread ID and the amout of time the emitter has been alive.
		float2 uv = float2(ThreadID.x / 256, AliveTime);
		float2 randomValue = normalize(RandomValuesTexture.SampleLevel(Sampler, uv, 0).xy);
		float3 randomDirection = float3(randomValue.x, 0, randomValue.y);
		randomDirection = mul(Rotation, float4(randomDirection, 0)).xyz;

		// Create a position from the base of the cone and the top of the cone using the random scalar value
		PartA.Position = WorldPosition;

		// Create our velocity for the particle
		PartA.Velocity = randomDirection * StartSpeed;

		// Set all our other values from the emitter
		PartA.Age = Lifetime;
		PartB.Lifetime = Lifetime;
		PartB.GravityScale = GravityScale;
		PartB.StartSize = StartSize;
		PartB.EndSize = EndSize;

		PartA.Flags = Flags;
		PartB.StartColor = StartColor;
		PartB.EndColor = EndColor;

		uint index = DeadList.Consume();

		ParticleABuffer[index] = PartA;
		ParticleBBuffer[index] = PartB;
	}
}