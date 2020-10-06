
// Struct representing the data we expect to receive from earlier pipeline stages
// - Should match the output of our corresponding vertex shader
// - The name of the struct itself is unimportant
// - The variable names don't have to match other shaders (just the semantics)
// - Each variable must have a semantic, which defines its usage
struct VertexToPixel
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
	float4 position		: SV_POSITION;
	float4 color		: COLOR;
	float3 normal		: NORMAL;
	float3 worldPos		: POSITION;
	float2 uv			: UV;
};

// Struct for passing in directional light data
struct DirectionalLight
{
	float3 AmbientColor;
	// There is padding here
	float3 DiffuseColor;
	// There is padding here too
	float3 Direction;
};

// Struct for passing in directional light data
struct PointLight
{
	float3 AmbientColor;
	// There is padding here
	float3 DiffuseColor;
	// There is padding here too
	float3 Position;
};

cbuffer ExternalData : register(b0)
{
	DirectionalLight dLight;
	DirectionalLight dLight2;
	DirectionalLight dLight3;
	PointLight pLight;
	float3 cameraPos;
	float specExponent;
}

Texture2D diffuseTexture : register(t0);
SamplerState samplerOptions : register(s0);

// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
float3 CalcLight(VertexToPixel input, float3 ambientColor, float3 diffuseColor, float3 direction, float3 surfaceColor)
{
	// Lighting calculations
	// Normalize needed vectors
	float3 normal = normalize(input.normal);
	float3 dir = normalize(direction);

	// Figure out how lit the object can be
	float3 lightAmount = saturate(dot(normal, dir));

	// Diffuse
	float3 lightColor = lightAmount * diffuseColor * surfaceColor;
	// Ambient
	lightColor += ambientColor * surfaceColor;

	// Specularity
	float3 V = normalize(cameraPos - input.worldPos);
	float3 R = reflect(dir, input.normal);
	float spec = pow(saturate(dot(R, V)), specExponent);
	lightColor += spec.xxx;

	// Return the light created by this light specifically
	return lightColor;
};

float4 main(VertexToPixel input) : SV_TARGET
{
	// Get surface color from the texture
	float3 surfaceColor = diffuseTexture.Sample(samplerOptions, input.uv).rgb;

	// Set up variable for all necessary lightings
	float3 finalColor = float3(0,0,0);

	// Calc Light for the directional lights
	finalColor += CalcLight(input, dLight.AmbientColor, dLight.DiffuseColor, -dLight.Direction, surfaceColor);
	finalColor += CalcLight(input, dLight2.AmbientColor, dLight2.DiffuseColor, -dLight2.Direction, surfaceColor);
	// Calc Light for the point light
	float3 pointDirection = pLight.Position - input.worldPos;
	finalColor += CalcLight(input, pLight.AmbientColor, pLight.DiffuseColor, pointDirection, surfaceColor);

	// Just return the input color
	// - This color (like most values passing through the rasterizer) is 
	//   interpolated for each pixel between the corresponding vertices 
	//   of the triangle we're rendering
	return float4(finalColor, 1);
}