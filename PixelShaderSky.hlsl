#include "ShaderIncludes.hlsli"

TextureCube cubeTex : register(t0);
SamplerState samplerOptions : register(s0);

float4 main(VertexToPixelSky input) : SV_TARGET
{
	// Get sampled point
	return cubeTex.Sample(samplerOptions, input.sampleDir);
}