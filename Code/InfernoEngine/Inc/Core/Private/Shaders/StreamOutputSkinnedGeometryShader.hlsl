struct GSOutput
{
	float4 Pos : SV_POSITION;
	float3 Norm : NORMAL;
	float3 Tangent : TANGENT;
	float2 uv : TEXTCOORD;
	float4 Influences : BONEINFLUENCES;
	float4 Weights : BONEWEIGHTS;
};

[maxvertexcount(1)]
void main(point GSOutput input[1], inout PointStream< GSOutput > output)
{
	GSOutput Out = input[0];

	output.Append(Out);
}