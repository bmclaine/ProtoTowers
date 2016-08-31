cbuffer World : register (b0)
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

float4 main( float4 pos : POSITION ) : SV_POSITION
{

	float4 outPos = mul(WorldMatrix, pos);
	outPos = mul(ViewMatrix, outPos);
	outPos = mul(ProjectionMatrix, outPos);

	return outPos;
}