Texture2D<float> DepthTexture : register(t0);
Texture2D ParticleTextures[50] : register(t1);

SamplerState Sampler : register(s0);

struct PSInput
{
	float2 Uv					    : TEXTCOORD1;
	float4 Color				    : COLOR;
};

float4 main(PSInput input) : SV_TARGET
{
	return ParticleTextures[0].Sample(Sampler, input.Uv) * input.Color;
}