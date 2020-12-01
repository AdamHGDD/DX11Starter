#include "ShaderIncludes.hlsli"

cbuffer ExternalData : register(b0)
{
	float pixelWidth;
	float pixelHeight;
	float depthAdjust;
	float normalAdjust;
}

// Textures in memory
Texture2D PixelsRender : register(t0);
Texture2D ShadowsRender : register(t1);
Texture2D NormalsRender : register(t2);
Texture2D DepthsRender : register(t3);

// Sampler
SamplerState samplerOptions	: register(s0);

// Main shader method
float4 main(VertexToPixelPP input) : SV_TARGET
{
	// Adjacent pixels
	float2 lPixel = input.uv + float2(-pixelWidth, 0);
	float2 rPixel = input.uv + float2(pixelWidth, 0);
	float2 dPixel = input.uv + float2(0, -pixelHeight);
	float2 uPixel = input.uv + float2(0, pixelHeight);

	return PixelsRender.Sample(samplerOptions, input.uv);
}