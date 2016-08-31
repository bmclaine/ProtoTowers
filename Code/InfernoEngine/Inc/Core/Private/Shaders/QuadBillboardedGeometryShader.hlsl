struct GSInput
{
};
struct GSOutput
{
	float2 uv : TEXTCOORD;
	float2 position : TEXTCOORD2;
	float4 pos : SV_POSITION;
};

cbuffer QuadInfo : register(b0)
{
	float2 Dimensions;
	float2 Pad;
}
cbuffer View : register (b1)
{
	float4x4 ViewMatrix;
}
cbuffer Projection : register (b2)
{
	float4x4 ProjectionMatrix;
}
cbuffer Position : register(b3)
{
	float3 Position;
	float PosPad;
}

[maxvertexcount(4)]
void main(point GSInput input[1], inout TriangleStream<GSOutput> output)
{
	GSOutput Output = (GSOutput)0;

	float3 vPos = mul(ViewMatrix, float4(Position, 1)).xyz;

	const float2 offsets[4] =
	{
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
	for (int i = 0; i < 4; i++)
	{
		float2 offset = offsets[i];
		float2 uv = uvs[i];
		
		float3 viewPos = vPos;
		viewPos.xy += float2(Dimensions.x * offset.x, Dimensions.y * offset.y);

		Output.pos = mul(ProjectionMatrix, float4(viewPos, 1));
		Output.position = offsets[i];
		Output.uv = uv;

		output.Append(Output);
	}
}