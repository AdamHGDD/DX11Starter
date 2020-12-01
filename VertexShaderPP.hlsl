#include "ShaderIncludes.hlsli"

VertexToPixelPP main( uint id : SV_VertexID)
{
	VertexToPixelPP output;

	// Create a right triangle
	// For 0: 0, 0
	// For 1: 2, 0
	// For 2: 0, 2
	output.uv = float2((id << 1) & 2, id & 2);

	// Move UV to position space
	output.position = float4(output.uv, 0, 1);
	output.position.x = output.position.x * 2 - 1;
	output.position.y = output.position.y * -2 + 1;

	return output;
}