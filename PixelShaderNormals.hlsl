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
Texture2D diffuseTexture : register(t0);
Texture2D normalsTexture : register(t1);

// Sampler state options like texture wrapping, and distance
SamplerState samplerOptions : register(s0);

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

// Main shader method
float4 main(VertexToPixelNormals input) : SV_TARGET
{
	// Get surface color from the texture
	float3 surfaceColor = diffuseTexture.Sample(samplerOptions, input.uv).rgb;

	// Set up variable for all necessary lightings
	float3 finalColor = float3(0,0,0);

	// Get normals to be from -1 to 1 instead of 0 to 1
	float3 vectorNormal = normalsTexture.Sample(samplerOptions, input.uv).rgb * 2 - 1;

	// Change normal to represent the normal map
	input.normal = mul(vectorNormal, GetTBN(input));

	// Calc Light for the directional lights
	finalColor += CalcLight(input, cameraPos, dLight.AmbientColor, dLight.DiffuseColor, -dLight.Direction, surfaceColor, specExponent);
	// Calc Light for the point light
	float3 pointDirection = pLight.Position - input.worldPos;
	finalColor += CalcLight(input, cameraPos, pLight.AmbientColor, pLight.DiffuseColor, pointDirection, surfaceColor, specExponent);

	// Just return the input color
	// - This color (like most values passing through the rasterizer) is 
	//   interpolated for each pixel between the corresponding vertices 
	//   of the triangle we're rendering
	return float4(finalColor, 1);
}