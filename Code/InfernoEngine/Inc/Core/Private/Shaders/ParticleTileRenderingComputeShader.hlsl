#define TILE_SIZE_X 32
#define TILE_SIZE_Y 32
#define NUM_TRHEADS_PER_TILE TILE_SIZE_X * TILE_SIZE_Y
#define MAX_PARTICLES_PER_TILE_FOR_RENDERING 500
#define PARTICLE_TILE_BUFFER_SIZE 1024

struct ParticlePartA
{
	float3 Position;
	uint   Flags;

	float3 Velocity;
	float  Age;

	float4 Color;
};

StructuredBuffer<ParticlePartA> ParticleBuffer : register(t0);
StructuredBuffer<float4> ViewSpacePositionAndRadius : register(t1);
Buffer<uint> TiledIndexBuffer : register(t2);

Texture2D<float> DepthMap : register(t3);
Texture2DArray ParticleTextures : register(t4);

RWBuffer<float4> OutputBuffer : register(u0);

SamplerState filter : register (s0);

// Tiling constants that are dependant on the screen resolution
cbuffer TilingConstantBuffer : register(b0)
{
	uint NumTilesX;
	uint NumTilesY;
	uint NumCoarseCullingTilesX;
	uint NumCoarseCullingTilesY;

	uint NumCullingTilesPerCoarseTileX;
	uint NumCullingTilesPerCoarseTileY;
	uint TilingPads[2];
};
// Constant buffers the shader will use
// This buffer is updated by the LightManager, so the ParticleManager doesn't need to
// worry about updating it
cbuffer WindowInformation : register(b1)
{
	float2 WindowBounds;
	uint NumLights;
	float Padding;
}
cbuffer InverseProjection : register(b2)
{
	float4x4 InvProjection;
}

groupshared float4 ParticleColor[MAX_PARTICLES_PER_TILE_FOR_RENDERING];
groupshared float3 ParticlePosition[MAX_PARTICLES_PER_TILE_FOR_RENDERING];
groupshared float  ParticleRadius[MAX_PARTICLES_PER_TILE_FOR_RENDERING];
groupshared uint   ParticleTextureID[MAX_PARTICLES_PER_TILE_FOR_RENDERING];
groupshared uint   NumParticles;

// Helper Functions
//-----------------------------------------------------------

void InitializeLDS(uint3 LocalID, uint3 GlobalID)
{
	uint LocalFlattenedID = LocalID.x + LocalID.y * TILE_SIZE_X;

	uint2 screenCoords = GlobalID.xy;
	uint2 TileID = screenCoords / uint2(TILE_SIZE_X, TILE_SIZE_Y);

	uint TileFlattenedID = TileID.x + TileID.y * NumTilesX;
	uint TileOffset = PARTICLE_TILE_BUFFER_SIZE * TileFlattenedID;

	if (LocalFlattenedID == 0)
		NumParticles = min(MAX_PARTICLES_PER_TILE_FOR_RENDERING, TiledIndexBuffer[TileOffset]);

	GroupMemoryBarrierWithGroupSync();

	uint numParticles = NumParticles;
	for (uint i = LocalFlattenedID; i < numParticles; i += NUM_TRHEADS_PER_TILE)
	{
		uint ParticleIndex = TiledIndexBuffer[TileOffset + 1 + i];

		ParticleColor[i] = ParticleBuffer[ParticleIndex].Color;
		ParticlePosition[i] = ViewSpacePositionAndRadius[ParticleIndex].xyz;
		ParticleRadius[i] = ViewSpacePositionAndRadius[ParticleIndex].w;
		ParticleTextureID[i] = (ParticleBuffer[ParticleIndex].Flags & 0xFF000000) >> 24; // TODO: Replace this with the proper texture number when it is added
	}

	GroupMemoryBarrierWithGroupSync();
}

// Helps find the intersection point of a ray on a plane
float3 CalcPointOnViewPlane(float3 PointOnPlane, float3 RayDir)
{
	float t = PointOnPlane.z / RayDir.z;
	float3 p = t * RayDir;
	return p;
}

float4 CalcParticleColor(uint ParticleIndex, float3 RayDir, float ViewSpaceDepth)
{
	float4 Color = ParticleColor[ParticleIndex];
	float3 Center = ParticlePosition[ParticleIndex];
	float  Radius = ParticleRadius[ParticleIndex];
	uint   TextureID = ParticleTextureID[ParticleIndex];

	float3 ViewSpacePosition = Center;

	[branch]
	if (ViewSpacePosition.z > ViewSpaceDepth)
	{
		Color = float4(0, 0, 0, 0);
		return Color;
	}

	float3 PointOnPlane = CalcPointOnViewPlane(ViewSpacePosition, RayDir);
	float2 uv = (PointOnPlane.xy - ViewSpacePosition.xy) / Radius;
	uv = 0.5f * uv + 0.5f;
	uv.y = 1 - uv.y;

	[branch]
	if (uv.x < 0 || uv.y < 0 || uv.x > 1 || uv.y > 1)
		Color = 0;
	else
		Color *= ParticleTextures.SampleLevel(filter, float3(uv, TextureID), 0);

	return Color;
}

float4 BlendFrontToBack(float3 ViewRay, float ViewSpaceDepth)
{
	uint numParts = NumParticles;
	float4 finalColor = 0;

	for (uint i = 0; i < numParts; i++)
	{
		float4 color = CalcParticleColor(i, ViewRay, ViewSpaceDepth);

		finalColor.xyz = (1 - finalColor.w) * (color.w * color.xyz) + finalColor.xyz;
		finalColor.w = color.w + (1 - color.w) * finalColor.w;

		// If we hit an alpha value of 1, break out since we are now opaque
		if (finalColor.w >= 1.0f)
		{
			finalColor.w = 1;
			break;
		}
	}

	return finalColor;
}

void EvaluateColorAtScreenCoord(uint2 ScreenCoord)
{
	float depth = DepthMap.Load(uint3(ScreenCoord.x, ScreenCoord.y, 0)).x;

	float2 screenCoord = (float2)ScreenCoord;
	screenCoord += 0.5f;

	float4 ViewSpacePosition;
	ViewSpacePosition.x = screenCoord.x / (float)WindowBounds.x;
	ViewSpacePosition.y = 1 - (screenCoord.y / (float)WindowBounds.y);
	ViewSpacePosition.xy = (2 * ViewSpacePosition.xy) - 1;
	ViewSpacePosition.z = depth;
	ViewSpacePosition.w = 1;

	ViewSpacePosition = mul(InvProjection, ViewSpacePosition);
	ViewSpacePosition.xyz /= ViewSpacePosition.w;

	float ViewSpaceDepth = ViewSpacePosition.z;
	float3 ViewRay = normalize(ViewSpacePosition.xyz);

	float4 color = BlendFrontToBack(ViewRay, ViewSpaceDepth);

	if (ScreenCoord.x > (uint)WindowBounds.x)
		ScreenCoord.x = (uint)WindowBounds.x;

	uint PixelLocation = ScreenCoord.x + (ScreenCoord.y * WindowBounds.x);

	OutputBuffer[PixelLocation] = color;
}

//-----------------------------------------------------------
// End Helper Functions

[numthreads(TILE_SIZE_X, TILE_SIZE_Y, 1)]
void main(uint3 LocalID : SV_GroupThreadID, uint3 GroupID : SV_GroupID, uint3 GlobalID : SV_DispatchThreadID)
{
	InitializeLDS(LocalID, GlobalID);

	EvaluateColorAtScreenCoord(GlobalID.xy);
}