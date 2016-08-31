struct GSInput
{
	float4 ViewSpaceAndRadius : TEXTCOORD0;
	float4 Color              : COLOR;
};

struct GSOutput
{
	float2 Uv					    : TEXTCOORD1;
	float4 Color				    : COLOR;
	float4 Pos					    : SV_POSITION;
};

//cbuffer BillboardRotation : register (b0)
//{
//	float4x4 Rotation;
//}
//cbuffer View : register (b1)
//{
//	float4x4 ViewMatrix;
//}
cbuffer Projection : register (b2)
{
	float4x4 ProjectionMatrix;
}

[maxvertexcount(4)]
void main(point GSInput input[1], inout TriangleStream<GSOutput> streamOut)
{
	GSOutput output = (GSOutput)0;

	const float2 offsets[4] = 
	{
		float2 (-1,  1),
		float2 ( 1,  1),
		float2 (-1, -1),
		float2 ( 1, -1),
	};

	[unroll]
	for (int i = 0; i < 4; i++)
	{
		float2 offset = offsets[i];
		float2 uv = (offset + 1) * 0.5f;
		float radius = input[0].ViewSpaceAndRadius.w;

		float3 viewPos = input[0].ViewSpaceAndRadius.xyz;
		viewPos.xy += radius * offset;

		output.Pos = mul(ProjectionMatrix, float4(viewPos, 1));
		output.Uv = uv;
		output.Color = input[0].Color;

		streamOut.Append(output);
	}
}