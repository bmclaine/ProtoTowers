Buffer<float4> RenderBuffer : register(t0);

cbuffer WindowInformation : register(b0)
{
	float2 WindowBounds;
	uint NumLights;
	float Padding;
}

float4 main(float4 Position : SV_POSITION) : SV_TARGET
{
	float x = Position.x - (WindowBounds.x / 2);
	float y = Position.y;

	uint pixelIndex = x + (y * WindowBounds.x);
	float4 color = RenderBuffer.Load(pixelIndex);

	return color;
}