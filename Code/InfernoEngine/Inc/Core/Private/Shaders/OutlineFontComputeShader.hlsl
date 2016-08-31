#include "D3DX_DXGIFormatConvert.inl"

Texture2D<float4> FontImage : register (t0);
RWTexture2D<float4> FinishedImage : register(u0);

cbuffer TextureInfo : register (b0)
{
	uint2 TextureDimensions;
	float  AlphaThreshold;
	int   BorderThickness;
}

bool IsEdgeCase(uint2 coord)
{
	uint2 lefttop = coord - BorderThickness;
	uint2 rightbottom = coord + BorderThickness;

	return (lefttop.x < 0 || lefttop.y < 0 ||
	rightbottom.x >= TextureDimensions.x || rightbottom.y > TextureDimensions.y);
}

[numthreads(32, 32, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	if (DTid.x < TextureDimensions.x && DTid.y < TextureDimensions.y)
	{
		uint2 Coordinates = uint2(DTid.x, DTid.y);
		float4 color = FontImage[Coordinates];
		float alpha = color.a;

		// Alpha should usually only be 0 or 1 since we shouldn't be using alpha'd text with DirectWrite, instead
		// it would be used as the color scalar in the pixel shader.
		if (alpha >= AlphaThreshold)
		{
			// If we are an edge case, we can just apply the boarder
			if (IsEdgeCase(Coordinates))
			{
				color.xyz = 0;
			}
			else
			{
				// Check the surrounding area around the pixel to see if we can determine we are with the boarder range
				//[loop]
				//for (int x = -BorderThickness; x <= BorderThickness; x++)
				//{
				//	[loop]
				//	for (int y = -BorderThickness; y <= BorderThickness; y++)
				//	{
				//		// Instead of doing an if check, we just multiply by the alpha value
				//		alpha *= FontImage[uint2(Coordinates + uint2(x, y))].a;
				//	}
				//}

				int x;
				int y;

				[loop]
				for (x = -BorderThickness; x <= BorderThickness; x++)
					alpha *= FontImage[uint2(Coordinates.x + x, Coordinates.y)].a;

				[loop]
				for (y = -BorderThickness; y <= BorderThickness; y++)
					alpha *= FontImage[uint2(Coordinates.x, Coordinates.y + y)].a;

				for (x = y = -BorderThickness; x <= BorderThickness; x++, y++)
					alpha *= FontImage[uint2(Coordinates.x + x, Coordinates.y + y)].a;

				for (x = -BorderThickness, y = BorderThickness; x <= BorderThickness; x++, y--)
					alpha *= FontImage[uint2(Coordinates.x + x, Coordinates.y + y)].a;

				color.xyz *= alpha;
			}
		}

		FinishedImage[Coordinates] = color;
	}
}