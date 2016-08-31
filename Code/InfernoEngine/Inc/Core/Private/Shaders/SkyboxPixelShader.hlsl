TextureCube CubeTexture : register(t0);
SamplerState filter : register(s0);

float4 main(float3 uvw : TEXTCOORD3D) : SV_TARGET
{
	return CubeTexture.Sample(filter, uvw);
}