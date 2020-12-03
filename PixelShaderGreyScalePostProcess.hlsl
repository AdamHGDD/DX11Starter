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

	// COMPARE DEPTHS --------------------------

	// Sample the depths of this pixel and the surrounding pixels
	float depthHere = DepthsRender.Sample(samplerOptions, input.uv).r;
	float depthLeft = DepthsRender.Sample(samplerOptions, lPixel).r;
	float depthRight = DepthsRender.Sample(samplerOptions, rPixel).r;
	float depthUp = DepthsRender.Sample(samplerOptions, dPixel).r;
	float depthDown = DepthsRender.Sample(samplerOptions, uPixel).r;

	// Calculate how the depth changes by summing the absolute values of the differences
	float depthChange =
		abs(depthHere - depthLeft) +
		abs(depthHere - depthRight) +
		abs(depthHere - depthUp) +
		abs(depthHere - depthDown);

	float depthTotal = pow(saturate(depthChange), depthAdjust);

	// COMPARE NORMALS --------------------------

	// Sample the normals of this pixel and the surrounding pixels
	float3 normalHere = NormalsRender.Sample(samplerOptions, input.uv).rgb;
	float3 normalLeft = NormalsRender.Sample(samplerOptions, lPixel).rgb;
	float3 normalRight = NormalsRender.Sample(samplerOptions, rPixel).rgb;
	float3 normalUp = NormalsRender.Sample(samplerOptions, dPixel).rgb;
	float3 normalDown = NormalsRender.Sample(samplerOptions, uPixel).rgb;

	// Calculate how the normal changes by summing the absolute values of the differences
	float3 normalChange =
		abs(normalHere - normalLeft) +
		abs(normalHere - normalRight) +
		abs(normalHere - normalUp) +
		abs(normalHere - normalDown);

	// Total the components
	float normalTotal = pow(saturate(normalChange.x + normalChange.y + normalChange.z), normalAdjust);

	// COMPARE Shadows --------------------------

	// Sample the normals of this pixel and the surrounding pixels
	float shadowHere = ShadowsRender.Sample(samplerOptions, input.uv).r;
	float shadowLeft = ShadowsRender.Sample(samplerOptions, lPixel).r;
	float shadowRight = ShadowsRender.Sample(samplerOptions, rPixel).r;
	float shadowUp = ShadowsRender.Sample(samplerOptions, dPixel).r;
	float shadowDown = ShadowsRender.Sample(samplerOptions, uPixel).r;

	// Calculate how the normal changes by summing the absolute values of the differences
	float3 shadowChange =
		abs(shadowHere - shadowLeft) +
		abs(shadowHere - shadowRight) +
		abs(shadowHere - shadowUp) +
		abs(shadowHere - shadowDown);

	// Total the components
	float shadowTotal = pow(saturate(shadowChange * 4), 5);

	// FINAL COLOR VALUE -----------------------------

	// Which result, depth or normal, is more impactful?
	float outline = max(max(depthTotal, normalTotal), shadowTotal);

	// Sample the color here
	float3 color = PixelsRender.Sample(samplerOptions, input.uv).rgb;
	color = ((color.r + color.g + color.b) / 3).rrr;

	// Interpolate between this color and the outline
	float3 finalColor = lerp(color, float3(0, 0, 0), outline);
	return float4(finalColor, 1);
}