
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

cbuffer ExternalData : register(b0)
{
	DirectionalLight dLight;
	DirectionalLight dLight2;
	DirectionalLight dLight3;
}

// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
float3 CalcLight(VertexToPixel input, DirectionalLight light)
{
	// Lighting calculations
	// Normalize needed vectors
	float3 normal = normalize(input.normal);
	float3 dir = normalize(-light.Direction);

	// Figure out how lit the object can be
	float3 lightAmount = saturate(dot(normal, dir));

	// Diffuse
	float3 lightColor = lightAmount * light.DiffuseColor * input.color;
	// Ambient
	lightColor += light.AmbientColor* input.color;

	// Return the light created by this light specifically
	return lightColor;
};

float4 main(VertexToPixel input) : SV_TARGET
{
	// Set up variable for all necessary lightings
	float3 finalColor = float3(0,0,0);

	finalColor += CalcLight(input, dLight);
	finalColor += CalcLight(input, dLight2);
	finalColor += CalcLight(input, dLight3);

	// Just return the input color
	// - This color (like most values passing through the rasterizer) is 
	//   interpolated for each pixel between the corresponding vertices 
	//   of the triangle we're rendering
	return float4(finalColor, 1);
}