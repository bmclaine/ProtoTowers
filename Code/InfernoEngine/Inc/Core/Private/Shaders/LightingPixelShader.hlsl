#define TILE_SIZE_X 16
#define TILE_SIZE_Y 16
#define MAX_LIGHTS_PER_TILE 50
#define BARRIER_VALUE 0xF7FFFFFF

Texture2D image : register(t0);
SamplerState filter : register(s0);

StructuredBuffer<uint> LightIndiceLists : register(t2);
StructuredBuffer<float4> LightCentersAndRadii : register(t3);
StructuredBuffer<float4> LightColorAndIntensity : register(t4);

cbuffer Color : register (b0)
{
	float4 color;
}
cbuffer Ambience : register (b1)
{
	float4 ambientColorAndIntensity;
}
cbuffer WindowInformation : register(b2)
{
	float2 WindowBounds;
	uint NumLights;
	float Padding;
}
cbuffer UVOffsets : register(b3)
{
	float2 offset;
	float2 tiling;
}

struct PS_IN
{
	float2 uv : TEXTCOORD;
	float4 position : SV_POSITION;
	float3 wPosition : POSITION;
	float3 normal : NORMAL;
};

uint GetTileIndex(float2 ScreenPos)
{
	float fTileResX = (float)TILE_SIZE_X;
	float fTileResY = (float)TILE_SIZE_Y;
	uint nNumCellsX = (WindowBounds.x + TILE_SIZE_X - 1) / TILE_SIZE_X;
	uint nTileIdx = floor(ScreenPos.x / fTileResX) + floor(ScreenPos.y / fTileResY)*nNumCellsX;
	return nTileIdx;
}

float4 main(PS_IN input) : SV_TARGET
{
	float4 diffuse = image.Sample(filter, (input.uv * tiling) + offset) * color;
	uint TileIndex = GetTileIndex(input.position.xy);
	uint Index = MAX_LIGHTS_PER_TILE * TileIndex;
	uint LightIndex = LightIndiceLists[Index];

	// Start our accumulated light to zero and our diffuse scaled by the ambient light
	float3 accumLight = float3(0, 0, 0); //ambientColorAndIntensity.xyz * ambientColorAndIntensity.w;
	diffuse.xyz *= ambientColorAndIntensity.xyz * ambientColorAndIntensity.w;
	float lamda = 0.0f;

	float3 Norm = normalize(input.normal);

	[loop]
	while (LightIndex != BARRIER_VALUE)
	{
		uint CurrIndex = LightIndex;
		Index++;
		LightIndex = LightIndiceLists[Index];
		
		float4 CenterAndRadius = LightCentersAndRadii[CurrIndex];
		
		float3 ToLight = CenterAndRadius.xyz - input.wPosition.xyz;
		float3 LightDir = normalize(ToLight);
		float LightDistance = length(ToLight);
		
		float3 LightDiffuse = float3(0, 0, 0);
		float falloff = 0.0f;
		
		if (LightDistance < CenterAndRadius.w)
		{
			float x = LightDistance / CenterAndRadius.w;
			float4 ColorAndIntensity = LightColorAndIntensity[CurrIndex];
			falloff = (1.0f - x) * ColorAndIntensity.w * saturate(dot(LightDir, Norm));
			LightDiffuse = ColorAndIntensity.rgb * falloff;
		}
		
		lamda = max(lamda, falloff);
		accumLight += LightDiffuse;
	}

	accumLight = saturate(accumLight);
	//lamda *= 0.75;
	diffuse.xyz = diffuse.xyz * (1.0f - lamda) + accumLight * lamda;
	return diffuse;
}