AppendStructuredBuffer<uint> DeadList : register (u0);

[numthreads(256, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	DeadList.Append(DTid.x);
}