#define NUM_PARTICLES_PER_TILE 1023
#define NUM_PARTICLES_FOR_SORTING 2046
#define TILE_SIZE_X 32
#define TILE_SIZE_Y 32

StructuredBuffer<float4> ViewSpacePositionAndRadius : register(t0);
Buffer<uint> CoarseBuffer : register(t1);
Buffer<uint> CoarseBufferCounters : register(t2);

Texture2D<float> DepthMap : register(t3);

RWBuffer<uint> TiledIndexBuffer : register(u0);

groupshared uint  MaxZDepth;
groupshared uint  ParticleIDs[NUM_PARTICLES_FOR_SORTING];
groupshared float ParticleDistances[NUM_PARTICLES_FOR_SORTING];
groupshared uint  NumParticles;

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
cbuffer ActiveList : register(b3)
{
	uint AliveCount;
	uint3 AlivePadding;
}

// Helper Functions
// ------------------------------------------------

// Creates a plane from 3 points, but we assume the third point is origin saving us on later calculations
float3 CreatePlane(float3 pt1, float3 pt2)
{
	return normalize(cross(pt1, pt2));
}

// Returns the signed distance from the plane, but since we assume we pass through origin, we can save calculations
float GetSignedDistanceFromPlane(float3 pt, float3 plNrm)
{
	return dot(pt, plNrm);
}

// convert a depth value from post-projection space into view space
float ConvertProjDepthToView(float z)
{
	z = 1.f / (z * InvProjection._43 + InvProjection._44);
	return z;
}

// Converts a point from Projection space to View Space
float4 ConvertProjectionToViewSpace(float4 pt)
{
	float4 nPt = mul(InvProjection, pt);
	nPt = nPt / nPt.w;

	return nPt;
}

// Converts a pixel x position to clip space
float pixelToClipX(float pixelx, float width)
{
	return (pixelx / width * 2.0f) - 1.0f;
}

// Converts a pixel y position to clip space
float pixelToClipY(float pixely, float height)
{
	return (height - pixely) / height * 2.0f - 1.0f;
}

// Calculates the depth of the current pixel and checks to see if it beats on of the bounds
void CalcMaxDepths(uint3 GlobalThreadID)
{
	float depth = DepthMap.Load(uint3(GlobalThreadID.x, GlobalThreadID.y, 0)).x;
	float viewDepth = ConvertProjDepthToView(depth);
	uint z = asuint(viewDepth);

	if (depth != 0.0f)
	{
		InterlockedMax(MaxZDepth, z);
	}
}

void CalcPlaneFrustums(uint2 GroupID, out float3 frustums[4])
{
	// Calculate our min and max x/y values
	float minx = GroupID.x * TILE_SIZE_X;
	float maxx = minx + TILE_SIZE_X;
	float miny = GroupID.y * TILE_SIZE_Y;
	float maxy = miny + TILE_SIZE_Y;

	// Calculate the 4 tile points starting TopLeft and going clockwise
	float3 TopLeft = ConvertProjectionToViewSpace(float4(pixelToClipX(minx, WindowBounds.x), pixelToClipY(miny, WindowBounds.y), 1.0f, 1.0f)).xyz;
	float3 TopRight = ConvertProjectionToViewSpace(float4(pixelToClipX(maxx, WindowBounds.x), pixelToClipY(miny, WindowBounds.y), 1.0f, 1.0f)).xyz;
	float3 BottomRight = ConvertProjectionToViewSpace(float4(pixelToClipX(maxx, WindowBounds.x), pixelToClipY(maxy, WindowBounds.y), 1.0f, 1.0f)).xyz;
	float3 BottomLeft = ConvertProjectionToViewSpace(float4(pixelToClipX(minx, WindowBounds.x), pixelToClipY(maxy, WindowBounds.y), 1.0f, 1.0f)).xyz;

	// Create our planes using the 4 points, keeping in mind that we are in left handed so we need to follow
	// the proper cross order giving us the positive side out.
	frustums[0] = CreatePlane(TopLeft, TopRight);
	frustums[1] = CreatePlane(TopRight, BottomRight);
	frustums[2] = CreatePlane(BottomRight, BottomLeft);
	frustums[3] = CreatePlane(BottomLeft, TopLeft);
}

void BitonicSort(uint LocalID)
{
	uint numParticles = NumParticles;

	// Round our number of particles to the nearest power of 2
	uint numParticlesPowerOfTwo = 1;
	while (numParticlesPowerOfTwo < numParticles)
		numParticlesPowerOfTwo <<= 1;

	GroupMemoryBarrierWithGroupSync();

	for (uint MergeSize = 2; MergeSize <= numParticlesPowerOfTwo; MergeSize = MergeSize * 2)
	{
		for (uint MergeSubSize = MergeSize >> 1; MergeSubSize > 0; MergeSubSize = MergeSubSize >> 1)
		{
			uint tempIndex = LocalID;
			uint indexLow = tempIndex & (MergeSubSize - 1);
			uint indexHigh = 2 * (tempIndex - indexLow);
			uint index = indexHigh + indexLow;

			uint SwapElement = MergeSubSize == MergeSize >> 1 ? indexHigh + (2 * MergeSubSize - 1) - indexLow : indexHigh + MergeSubSize + indexLow;
			if (SwapElement < numParticles && index < numParticles)
			{
				if (ParticleDistances[index] > ParticleDistances[SwapElement])
				{
					uint uTemp = ParticleIDs[index];
					float vTemp = ParticleDistances[index];

					ParticleIDs[index] = ParticleIDs[SwapElement];
					ParticleDistances[index] = ParticleDistances[SwapElement];

					ParticleIDs[SwapElement] = uTemp;
					ParticleDistances[SwapElement] = vTemp;
				}
			}

			GroupMemoryBarrierWithGroupSync();
		}
	}
}

// Get the number of particles in this coarse bin
uint GetNumParticlesInCoarseTile(uint tile)
{
	return CoarseBufferCounters[tile];
}

// Get the global particle index from this bin
uint GetParticleIndexFromCoarseBuffer(uint binIndex, uint listIndex)
{
	uint offset = binIndex * AliveCount;
	return CoarseBuffer[offset + listIndex];
}

void AddParticleToVisibleList(uint index, float distance)
{
	uint Counter = 0;
	InterlockedAdd(NumParticles, 1, Counter);

	if (Counter < NUM_PARTICLES_FOR_SORTING)
	{
		ParticleIDs[Counter] = index;
		ParticleDistances[Counter] = distance;
	}
}

// ------------------------------------------------
// End Helper Functions

[numthreads(TILE_SIZE_X, TILE_SIZE_Y, 1)]
void main(uint3 LocalID : SV_GroupThreadID, uint3 GroupID : SV_GroupID, uint3 GlobalID : SV_DispatchThreadID )
{
	uint LocalFlattenedID = LocalID.x + (LocalID.y * TILE_SIZE_X);

	if (LocalFlattenedID == 0)
	{
		NumParticles = 0;
		MaxZDepth = 0;
	}

	GroupMemoryBarrierWithGroupSync();

	CalcMaxDepths(GlobalID);

	GroupMemoryBarrierWithGroupSync();
	float maxZ = asfloat(MaxZDepth);

	float3 frustums[4];
	CalcPlaneFrustums(GroupID.xy, frustums);

	uint TileX = GroupID.x / NumCullingTilesPerCoarseTileX;
	uint TileY = GroupID.y / NumCullingTilesPerCoarseTileY;

	uint coarseTileID = TileX + TileY * NumCoarseCullingTilesX;
	uint numParticles = GetNumParticlesInCoarseTile(coarseTileID);

	uint i;
	for (i = LocalFlattenedID; i < numParticles; i += TILE_SIZE_X * TILE_SIZE_Y)
	{
		uint index = GetParticleIndexFromCoarseBuffer(coarseTileID, i);
		float4 Position = ViewSpacePositionAndRadius[index];
		float3 Center = Position.xyz;
		float  Radius = Position.w;

		// Do depth culling
		if (Center.z - maxZ < Radius)
		{
			if (GetSignedDistanceFromPlane(Center, frustums[0]) < Radius &&
				GetSignedDistanceFromPlane(Center, frustums[1]) < Radius &&
				GetSignedDistanceFromPlane(Center, frustums[2]) < Radius &&
				GetSignedDistanceFromPlane(Center, frustums[3]) < Radius)
			{
				AddParticleToVisibleList(index, Position.z);
			}
		}
	}

	GroupMemoryBarrierWithGroupSync();

	BitonicSort(LocalFlattenedID);

	if (LocalFlattenedID == 0)
		NumParticles = min(NumParticles, NUM_PARTICLES_PER_TILE);

	GroupMemoryBarrierWithGroupSync();

	uint TileFlattenedID = GroupID.x + GroupID.y * NumTilesX;
	uint TiledBufferOffset = (NUM_PARTICLES_PER_TILE + 1) * TileFlattenedID;

	numParticles = NumParticles;

	for (i = LocalFlattenedID; i < numParticles; i += TILE_SIZE_X * TILE_SIZE_Y)
		TiledIndexBuffer[TiledBufferOffset + 1 + i] = ParticleIDs[i];

	if (LocalFlattenedID == 0)
		TiledIndexBuffer[TiledBufferOffset] = numParticles;
}