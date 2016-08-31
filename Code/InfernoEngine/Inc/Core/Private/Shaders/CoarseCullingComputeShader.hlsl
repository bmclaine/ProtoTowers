#define NUM_TILES_X 16
#define NUM_TILES_Y 8
#define NUM_TILES NUM_TILES_X * NUM_TILES_Y
#define CULLING_TILE_SIZE_X 32
#define CULLING_TILE_SIZE_Y 32

StructuredBuffer<float4> ViewSpacePositionAndRadius : register(t0);
StructuredBuffer<float2> IndexBuffer : register(t1);

RWBuffer<uint> TiledIndexBuffer : register(u0);
RWBuffer<uint> TiledBufferCounters : register(u1);

groupshared float3 FrustumData[NUM_TILES_X][NUM_TILES_Y][4];

cbuffer ActiveList : register(b0)
{
	uint AliveCount;
	uint3 AlivePadding;
}
// Tiling constants that are dependant on the screen resolution
cbuffer TilingConstantBuffer : register(b1)
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
cbuffer WindowInformation : register(b2)
{
	float2 WindowBounds;
	uint NumLights;
	float Padding;
}
cbuffer InverseProjection : register(b3)
{
	float4x4 InvProjection;
}

// Helper Functions
// --------------------------------------------------------------

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

void CalcFrustumsForLDS(uint localIDx)
{
	if (localIDx < NUM_TILES)
	{
		uint TileWidth = NumCullingTilesPerCoarseTileX * CULLING_TILE_SIZE_X;
		uint TileHeight = NumCullingTilesPerCoarseTileY * CULLING_TILE_SIZE_Y;

		uint TileIDX = localIDx % NUM_TILES_X;
		uint TileIDY = localIDx / NUM_TILES_X;

		uint minx = TileIDX * TileWidth;
		uint maxx = minx + TileWidth;
		uint miny = TileIDY * TileHeight;
		uint maxy = miny + TileHeight;

		float3 frustums[4];
		frustums[0] = ConvertProjectionToViewSpace(float4(pixelToClipX(minx, WindowBounds.x), pixelToClipY(miny, WindowBounds.y), 1.0f, 1.0f)).xyz;
		frustums[1] = ConvertProjectionToViewSpace(float4(pixelToClipX(maxx, WindowBounds.x), pixelToClipY(miny, WindowBounds.y), 1.0f, 1.0f)).xyz;
		frustums[2] = ConvertProjectionToViewSpace(float4(pixelToClipX(maxx, WindowBounds.x), pixelToClipY(maxy, WindowBounds.y), 1.0f, 1.0f)).xyz;
		frustums[3] = ConvertProjectionToViewSpace(float4(pixelToClipX(minx, WindowBounds.x), pixelToClipY(maxy, WindowBounds.y), 1.0f, 1.0f)).xyz;

		for (uint i = 0; i < 4; i++)
			FrustumData[TileIDX][TileIDY][i] = CreatePlane(frustums[i], frustums[(i + 1) & 3]);
	}
}

void AddToList(uint bufferID, uint GlobalIndex)
{
	uint Counter = 0;
	InterlockedAdd(TiledBufferCounters[bufferID], 1, Counter);

	Counter += bufferID * AliveCount;
	TiledIndexBuffer[Counter] = GlobalIndex;
}

// --------------------------------------------------------------
// End Helper Functions

[numthreads(256, 1, 1)]
void main(uint3 LocalID : SV_GroupThreadID, uint3 GroupID : SV_GroupID, uint3 GlobalID : SV_DispatchThreadID)
{
	CalcFrustumsForLDS(LocalID.x);

	if (GlobalID.x < NUM_TILES)
		TiledBufferCounters[GlobalID.x] = 0;

	GroupMemoryBarrierWithGroupSync();

	if (GlobalID.x < AliveCount)
	{
		uint index = (uint)IndexBuffer[GlobalID.x].y;

		float4 Position = ViewSpacePositionAndRadius[index];
		float3 Center = Position.xyz;
		float  Radius = Position.w;

		// Do a near plane test
		if (-Center.z < Radius)
		{
			for (int x = 0; x < NUM_TILES_X; x++)
			{
				for (int y = 0; y < NUM_TILES_Y; y++)
				{
					// Check against each frustum
					if (GetSignedDistanceFromPlane(Center, FrustumData[x][y][0]) < Radius &&
						GetSignedDistanceFromPlane(Center, FrustumData[x][y][1]) < Radius &&
						GetSignedDistanceFromPlane(Center, FrustumData[x][y][2]) < Radius &&
						GetSignedDistanceFromPlane(Center, FrustumData[x][y][3]) < Radius)
					{
						// If we passed all the tests, add us to the bin
						AddToList(y * NUM_TILES_X + x, index);
					}
				}
			}
		}
	}
}