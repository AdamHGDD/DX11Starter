#include "ShaderIncludes.hlsli"

cbuffer ExternalData : register(b0)
{
	DirectionalLight dLight;
	PointLight pLight;
	float3 cameraPos;
	float specExponent;
}

Texture2D Albedo : register(t0);
Texture2D RoughnessMap : register(t1);
Texture2D MetalnessMap : register(t2);
SamplerState samplerOptions : register(s0);

float4 main(VertexToPixel input) : SV_TARGET
{
	// Get surface color from the texture
	float3 surfaceColor = pow(Albedo.Sample(samplerOptions, input.uv).rgb, 2.2);

	// Set up variable for all necessary lightings
	float3 finalColor = float3(0,0,0);

	// Rough and Metal
	float roughness = RoughnessMap.Sample(samplerOptions, input.uv).r;
	float metalness = MetalnessMap.Sample(samplerOptions, input.uv).r;

	// Calc Light for the directional lights
	finalColor += CalcLight(input, cameraPos, dLight.AmbientColor, dLight.DiffuseColor, -dLight.Direction, surfaceColor, roughness, metalness);
	// Calc Light for the point light
	float3 pointDirection = pLight.Position - input.worldPos;
	finalColor += CalcLight(input, cameraPos, pLight.AmbientColor, pLight.DiffuseColor, pointDirection, surfaceColor, roughness, metalness);

	// Just return the input color
	// - This color (like most values passing through the rasterizer) is 
	//   interpolated for each pixel between the corresponding vertices 
	//   of the triangle we're rendering
	return float4(pow(finalColor, 1.0f / 2.2f), 1);
}