struct GSInput
{
};
struct GSOutput
{
	float2 uv : TEXTCOORD;
	float2 position : TEXTCOORD2;
	float4 pos : SV_POSITION;
};

cbuffer World : register(b0)
{
	float4x4 WorldMatrix;
}
cbuffer View : register (b1)
{
	float4x4 ViewMatrix;
}
cbuffer Projection : register (b2)
{
	float4x4 ProjectionMatrix;
}

[maxvertexcount(4)]
void main(point GSInput input[1], inout TriangleStream<GSOutput> output)
{
	const float2 pos[4] = {
		float2 (-1, 1),
		float2 (1, 1),
		float2 (-1, -1),
		float2 (1, -1),
	};
	const float2 uvs[4] =
	{
		float2 (0, 0),
		float2 (1, 0),
		float2 (0, 1),
		float2 (1, 1),
	};

	[unroll]
	for (uint i = 0; i < 4; i++)
	{
		GSOutput element = (GSOutput)0;
		element.uv = uvs[i];
		element.position = pos[i];
		element.pos = mul(WorldMatrix, float4(pos[i], 0, 1));
		element.pos = mul(ViewMatrix, element.pos);
		element.pos = mul(ProjectionMatrix, element.pos);
		output.Append(element);
	}
}