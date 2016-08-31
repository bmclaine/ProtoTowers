struct GSOutput
{
	float4 pos : SV_POSITION;
};

[maxvertexcount(4)]
void main(point float4 input[1] : SV_POSITION, inout TriangleStream<GSOutput> output)
{
	GSOutput element = (GSOutput)0;

	element.pos = float4(-1, 1, 0, 1);
	output.Append(element);

	element.pos = float4(1, 1, 0, 1);
	output.Append(element);

	element.pos = float4(-1, -1, 0, 1);
	output.Append(element);

	element.pos = float4(1, -1, 0, 1);
	output.Append(element);

	output.RestartStrip();
}