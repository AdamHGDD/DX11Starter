#ifndef __GGP_SHADER_INCLUDES__
#define __GGP_SHADER_INCLUDES__

// My code

// Struct representing a single vertex worth of data
// - This should match the vertex definition in our C++ code
// - By "match", I mean the size, order and number of members
// - The name of the struct itself is unimportant, but should be descriptive
// - Each variable must have a semantic, which defines its usage
struct VertexShaderInput
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
	float3 position		: POSITION;     // XYZ position
	float3 normal		: NORMAL;
	float3 tangent		: TANGENT;
	float2 uv		: UV;
};

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

// New version for normals
struct VertexToPixelNormals
{
	// Data type	Name	Semantic
	float4 position		: SV_POSITION;
	float4 color		: COLOR;
	float3 normal		: NORMAL;
	float3 tangent		: TANGENT;
	float3 worldPos		: POSITION;
	float2 uv			: UV;
};

// New version for normals
struct VertexToPixelSky
{
	// Data type	Name	Semantic
	float4 position		: SV_POSITION;
	float3 sampleDir		: DIRECTION;
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

// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
float3 CalcLight(VertexToPixel input, float3 cameraPos, float3 ambientColor, float3 diffuseColor, float3 direction, float3 surfaceColor, float specExp)
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

	// Specularity vectors
	float3 V = normalize(cameraPos - input.worldPos);
	float3 R = reflect(dir, normalize(input.normal));
	// Specularity calculation
	float spec = pow(saturate(dot(R, V)), specExp);
	lightColor += spec.xxx;

	// Return the light created by this light specifically
	return lightColor;
};
float3 CalcLight(VertexToPixelNormals input, float3 cameraPos, float3 ambientColor, float3 diffuseColor, float3 direction, float3 surfaceColor, float specExp)
{
	// Lighting calculations
	// Normalize needed vectors
	float3 normal = normalize(input.normal);
	float3 dir = normalize(direction);

	// Figure out how lit the object can be
	float3 lightAmount = saturate(dot(normal, dir));

	// Diffuse
	float3 diffuse = lightAmount * diffuseColor * surfaceColor;
	float3 lightColor = diffuse;
	// Ambient
	float3 ambient = ambientColor * surfaceColor;
	lightColor += ambient;

	// Specularity vectors
	float3 V = normalize(cameraPos - input.worldPos);
	float3 R = reflect(dir, normalize(input.normal));
	// Specularity calculation
	float spec = pow(saturate(dot(R, V)), specExp); 
	// Spec = 0 if diffuse = 0
	spec *= any(diffuse);
	lightColor += spec.xxx;

	// Return the light created by this light specifically
	return lightColor;
};


#endif