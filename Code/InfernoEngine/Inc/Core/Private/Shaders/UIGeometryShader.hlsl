struct GSInput 
{
	float4 pos : SV_POSITION;
};

struct GSOutput
{
	float2 uv : TEXTCOORD;
	float2 position : TEXTCOORD2;
	float4 pos : SV_POSITION;
};

cbuffer CanvasDetails : register (b0)
{
	float2 CanvasX : XDIMENSIONS;
	float2 CanvasY : YDIMENSIONS;
	float2 ImageU : UDIMENSIONS;
	float2 ImageV : VDIMENSIONS;
}

cbuffer ScaleFactor : register (b1)
{
	float2 Scale : SCALEFACTOR;
	float2 Dimensions : SCREENDIMENSIONS;
}

cbuffer RadialOriginInfo : register (b2)
{
	float2 TopLeft;
	float2 TopRight;
	float2 BottomLeft;
	float2 BottomRight;
};

cbuffer RotationInfo : register (b3)
{
	float rotation;
	float3 rotPadding;
}

float ToNDCX(float pixelPos)
{
	pixelPos = (pixelPos / (Dimensions.x * 0.5f)) - 1.0f;
	
	return pixelPos;
}
float ToNDCY(float pixelPos)
{
	pixelPos = -((pixelPos / (Dimensions.y * 0.5f)) - 1.0f);

	return pixelPos;
}

[maxvertexcount(4)]
void main(point GSInput input[1], inout TriangleStream< GSOutput > output)
{
	// Scale the Canvas Screen Positions by the Scale Factor
	float2 NDCPosX = CanvasX * Scale.x;
	float2 NDCPosY = CanvasY * Scale.y;

	// Take the Screen Position and turn it to NDC Space
	NDCPosX.x = ToNDCX(NDCPosX.x);
	NDCPosX.y = ToNDCX(NDCPosX.y);
	NDCPosY.x = ToNDCY(NDCPosY.x);
	NDCPosY.y = ToNDCY(NDCPosY.y);

	if (rotation == 0.0f)
	{
		float c = cos(rotation);
		float s = sin(rotation);
		
		float2x2 RotationMatrix = float2x2(c, -s, s, c);

		NDCPosX = mul(RotationMatrix, NDCPosX);
		NDCPosY = mul(RotationMatrix, NDCPosY);
	}

	GSOutput Outputs[4];
	Outputs[0].uv = float2(ImageU.x, ImageV.x);
	Outputs[0].position = TopLeft;
	Outputs[0].pos = float4(NDCPosX.x, NDCPosY.x, 0, 1);
	output.Append(Outputs[0]);

	Outputs[1].uv = float2(ImageU.y, ImageV.x);
	Outputs[1].position = TopRight;
	Outputs[1].pos = float4(NDCPosX.y, NDCPosY.x, 0, 1);
	output.Append(Outputs[1]);

	Outputs[2].uv = float2(ImageU.x, ImageV.y);
	Outputs[2].position = BottomLeft;
	Outputs[2].pos = float4(NDCPosX.x, NDCPosY.y, 0, 1);
	output.Append(Outputs[2]);

	Outputs[3].uv = float2(ImageU.y, ImageV.y);
	Outputs[3].position = BottomRight;
	Outputs[3].pos = float4(NDCPosX.y, NDCPosY.y, 0, 1);
	output.Append(Outputs[3]);
}