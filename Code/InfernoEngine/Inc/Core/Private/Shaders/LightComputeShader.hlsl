// Some needed constants
#define FLT_MAX 3.402823466e+38F
#define MAX_LIGHTS_PER_TILE 50

// Random constant used as to label the end of a list
#define BARRIER_VALUE 0xF7FFFFFF

// This must match what's on the cpu side
#define TILE_SIZE_X 16
#define TILE_SIZE_Y 16

StructuredBuffer<float4> LightCentersAndRadii : register(t0);
RWStructuredBuffer<uint> LightIndiceLists : register(u0);

Texture2D<float> DepthMap : register(t1);

// Constant buffers the shader will use
cbuffer WindowInformation : register(b0)
{
	float2 WindowBounds;
	uint NumLights;
	float Padding;
}

cbuffer InverseProjection : register(b1)
{
	float4x4 InvProjection;
}

cbuffer WorldView : register(b2)
{
	float4x4 WorldViewMatrix;
}

groupshared uint MaxZDepth;
groupshared uint MinZDepth;
groupshared uint LightCounter;
groupshared uint LightIndices[MAX_LIGHTS_PER_TILE];

// Helper functions
// These were referenced from the AMD example
// -------------------------------------------------------------------

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

bool TestAgainstFrustum(float3 pt, float radius, float3 plNrm1, float3 plNrm2, float3 plNrm3, float3 plNrm4)
{
	// Get the distnace from all 4 planes and test to see if we intersect or are in the planes
	bool test1 = GetSignedDistanceFromPlane(pt, plNrm1) < radius;
	bool test2 = GetSignedDistanceFromPlane(pt, plNrm2) < radius;
	bool test3 = GetSignedDistanceFromPlane(pt, plNrm3) < radius;
	bool test4 = GetSignedDistanceFromPlane(pt, plNrm4) < radius;

	// Return the results of the tests, only true if all true
	return (test1 && test2 && test3 && test4);
}

uint NumXTiles() { return (WindowBounds.x + TILE_SIZE_X - 1) / (float)TILE_SIZE_X; }
uint NumYTiles() { return (WindowBounds.y + TILE_SIZE_Y - 1) / (float)TILE_SIZE_Y; }

// Converts a point from Projection space to View Space
float4 ConvertProjectionToViewSpace(float4 pt)
{
	float4 nPt = mul(InvProjection, pt);
	nPt = nPt / nPt.w;

	return nPt;
}

// convert a depth value from post-projection space into view space
float ConvertProjDepthToView(float z)
{
	z = 1.f / (z * InvProjection._43 + InvProjection._44);
	return z;
}

// Calculates the depth of the current pixel and checks to see if it beats on of the bounds
void CalcMinMaxDepths(uint3 GlobalThreadID)
{
	float depth = DepthMap.Load(uint3(GlobalThreadID.x, GlobalThreadID.y, 0)).x;
	float viewDepth = ConvertProjDepthToView(depth);
	uint z = asuint(viewDepth);

	if (depth != 1.0f)
	{
		InterlockedMax(MaxZDepth, z);
		InterlockedMin(MinZDepth, z);
	}
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
// End Hepler Functions
// -------------------------------------------------------------------------------

bool returnTrue()
{
	return true;
}

#define NUM_X_THREADS TILE_SIZE_X
#define NUM_Y_THREADS TILE_SIZE_Y
#define THREADS_PER_TILE NUM_X_THREADS * NUM_Y_THREADS

[numthreads(NUM_X_THREADS, NUM_Y_THREADS, 1)]
void main(uint3 GlobalID : SV_DispatchThreadID, uint3 LocalID : SV_GroupThreadID, uint3 GroupID : SV_GroupID)
{
	// Creates a local index for a thread
	uint LocalIndex = LocalID.x + LocalID.y * NUM_X_THREADS;

	// If we are the first thread in the group, initialize the shared group memory
	if (LocalIndex == 0)
	{
		MinZDepth = 0xFFFFFFFF;
		MaxZDepth = 0;
		LightCounter = 0;
	}

	// Create our 4 planes for the current tile
	float3 plNrm1, plNrm2, plNrm3, plNrm4;
	{
		// Calculate our min and max x/y values
		float minx = GroupID.x * TILE_SIZE_X;
		float maxx = minx + TILE_SIZE_X;
		float miny = GroupID.y * TILE_SIZE_Y;
		float maxy = miny + TILE_SIZE_Y;

		// Create a width that is divisible evenly by our TileSize, this ensures we get the proper points
		// for our tiles when we calculate them.
		float WidthDivisibleByTileSize = TILE_SIZE_X * NumXTiles();
		float HeightDivisibleByTileSize = TILE_SIZE_Y * NumYTiles();
		
		// Calculate the 4 tile points starting TopLeft and going clockwise
		float3 TopLeft     = ConvertProjectionToViewSpace(float4(pixelToClipX(minx, WidthDivisibleByTileSize), pixelToClipY(miny, HeightDivisibleByTileSize), 1.0f, 1.0f)).xyz;
		float3 TopRight    = ConvertProjectionToViewSpace(float4(pixelToClipX(maxx, WidthDivisibleByTileSize), pixelToClipY(miny, HeightDivisibleByTileSize), 1.0f, 1.0f)).xyz;
		float3 BottomRight = ConvertProjectionToViewSpace(float4(pixelToClipX(maxx, WidthDivisibleByTileSize), pixelToClipY(maxy, HeightDivisibleByTileSize), 1.0f, 1.0f)).xyz;
		float3 BottomLeft  = ConvertProjectionToViewSpace(float4(pixelToClipX(minx, WidthDivisibleByTileSize), pixelToClipY(maxy, HeightDivisibleByTileSize), 1.0f, 1.0f)).xyz;

		// Create our planes using the 4 points, keeping in mind that we are in left handed so we need to follow
		// the proper cross order giving us the positive side out.
		plNrm1 = CreatePlane(TopLeft, TopRight);
		plNrm2 = CreatePlane(TopRight, BottomRight);
		plNrm3 = CreatePlane(BottomRight, BottomLeft);
		plNrm4 = CreatePlane(BottomLeft, TopLeft);
	}

	GroupMemoryBarrierWithGroupSync();

	CalcMinMaxDepths(GlobalID);

	GroupMemoryBarrierWithGroupSync();

	float minz = asfloat(MinZDepth);
	float maxz = asfloat(MaxZDepth);

	for (uint i = LocalIndex; i < NumLights; i += THREADS_PER_TILE)
	{
		float4 center = LightCentersAndRadii[i];
		float radius = center.w;
		center.xyz = mul(WorldViewMatrix, float4(center.xyz, 1)).xyz;

		if (TestAgainstFrustum(center.xyz, radius, plNrm1, plNrm2, plNrm3, plNrm4))
		{
			if (-center.z + minz < radius && center.z - maxz < radius)
			{
				uint tIndex = 0;
				InterlockedAdd(LightCounter, 1, tIndex);

				if (tIndex < MAX_LIGHTS_PER_TILE)
					LightIndices[tIndex] = i;
			}
		}
	}

	GroupMemoryBarrierWithGroupSync();

	uint GlobalIndex = GroupID.x + GroupID.y * NumXTiles();
	uint Offset = MAX_LIGHTS_PER_TILE * GlobalIndex;

	for (uint j = 0; j < LightCounter; j++)
		LightIndiceLists[Offset + j] = LightIndices[j];

	if (LocalIndex == 0)
		LightIndiceLists[Offset + LightCounter] = BARRIER_VALUE;
}