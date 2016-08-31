struct V_OUT
{
	float3 uvw : TEXTCOORD3D;
	float4 pos : SV_POSITION;
};

cbuffer View : register (b1)
{
	float4x4 ViewMatrix;
}
cbuffer Projection : register (b2)
{
	float4x4 ProjectionMatrix;
}

V_OUT main( float4 pos : POSITION )
{
	V_OUT output = (V_OUT)0;

	float4x4 view = ViewMatrix;
	view[0][3] = 0; view[1][3] = 0; view[2][3] = 0;

	output.uvw = pos.xyz;
	output.pos = mul(view, pos);
	output.pos = mul(ProjectionMatrix, output.pos).xyww;

	return output;
}