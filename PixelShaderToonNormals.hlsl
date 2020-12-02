#include "ShaderIncludes.hlsli"

// Most external data
cbuffer ExternalData : register(b0)
{
	DirectionalLight dLight;
	PointLight pLight;
	float3 cameraPos;
	float specExponent;
}

// Textures in memory
Texture2D Albedo : register(t0);
Texture2D RoughnessMap : register(t1);
Texture2D MetalnessMap : register(t2);
Texture2D NormalMap : register(t3);
Texture2D ShadowChart : register(t4);

// Sampler state options like texture wrapping, and distance
SamplerState samplerOptions : register(s0);
SamplerState samplerState2 : register(s1);

// Helper method for get TBN matrix
float3x3 GetTBN(VertexToPixelNormals input)
{
	// Normalized normal
	float3 N = normalize(input.normal);
	// Normalized tangent with anything along the Nth axis removed
	float3 T = normalize(input.tangent - N * dot(input.tangent, N));
	// Gram-Schmidt orthogonalization
	float3 B = cross(T, N);
	// Return the matrix
	return float3x3(T, B, N);
}

struct Output
{
	float4 color	: SV_TARGET0;
	float4 shadows	: SV_TARGET1;
	float4 normals	: SV_TARGET2;
	float depth : SV_TARGET3;
};

// Main shader method
Output main(VertexToPixelNormals input) : SV_TARGET
{
	// Create object for output
	Output output;

	// Get surface color from the texture
	float3 surfaceColor = pow(Albedo.Sample(samplerOptions, input.uv).rgb, 2.2);

	// Set up variable for all necessary lightings
	float3 finalColor = float3(0,0,0);
	float3 shadowColor = float3(0, 0, 0);

	// Rough and Metal
	float roughness = RoughnessMap.Sample(samplerOptions, input.uv).r;
	float metalness = MetalnessMap.Sample(samplerOptions, input.uv).r;
	float shadows = ShadowChart.Sample(samplerOptions, input.uv).r;

	// Get normals to be from -1 to 1 instead of 0 to 1
	float3 vectorNormal = NormalMap.Sample(samplerOptions, input.uv).rgb * 2 - 1;

	// Change normal to represent the normal map
	input.normal = mul(vectorNormal, GetTBN(input));

	// Calc Light for the directional lights
	finalColor += CalcLight(input, cameraPos, dLight.AmbientColor, dLight.DiffuseColor, -dLight.Direction, surfaceColor, roughness, metalness, ShadowChart, samplerState2);
	// Calc Light for the point light
	float3 pointDirection = pLight.Position - input.worldPos;
	finalColor += CalcLight(input, cameraPos, pLight.AmbientColor, pLight.DiffuseColor, pointDirection, surfaceColor, roughness, metalness, ShadowChart, samplerState2);

	// Mimic this for the shadows
	shadowColor += CalcLight(input, cameraPos, dLight.AmbientColor, dLight.DiffuseColor, -dLight.Direction, float3(1, 1, 1), 0, 0, ShadowChart, samplerState2);
	// Calc Light for the point light
	shadowColor += CalcLight(input, cameraPos, pLight.AmbientColor, pLight.DiffuseColor, pointDirection, float3(1, 1, 1), 0, 0, ShadowChart, samplerState2);


	// Just return the input color
	// - This color (like most values passing through the rasterizer) is 
	//   interpolated for each pixel between the corresponding vertices 
	//   of the triangle we're rendering
	output.color = float4(pow(finalColor, 1.0f / 2.2f), 1);
	output.normals = float4(input.normal, 0);
	output.depth = 0.5f;
	output.shadows = float4(pow(shadowColor, 1.0f / 2.2f), 1);

	return output;
}