#ifndef __GGP_SHADER_INCLUDES__
#define __GGP_SHADER_INCLUDES__

// PBR constants
// The fresnel value for non-metals (dielectrics)
// Page 9: "F0 of nonmetals is now a constant 0.04"
// http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf
static const float F0_NON_METAL = 0.04f;

// Minimum roughness for when spec distribution function denominator goes to zero
static const float MIN_ROUGHNESS = 0.0000001f; // 6 zeros after decimal

// Handy to have this as a constant
static const float PI = 3.14159265359f;

// PBR functions
// Lambert diffuse BRDF - Same as the basic lighting diffuse calculation!
// - NOTE: this function assumes the vectors are already NORMALIZED!
float DiffusePBR(float3 normal, float3 dirToLight)
{
	return saturate(dot(normal, dirToLight));
}


// Calculates diffuse amount based on energy conservation
//
// diffuse - Diffuse amount
// specular - Specular color (including light color)
// metalness - surface metalness amount
//
// Metals should have an albedo of (0,0,0)...mostly
// See slide 65: http://blog.selfshadow.com/publications/s2014-shading-course/hoffman/s2014_pbs_physics_math_slides.pdf
float3 DiffuseEnergyConserve(float diffuse, float3 specular, float metalness)
{
	return diffuse * ((1 - saturate(specular)) * (1 - metalness));
}

// GGX (Trowbridge-Reitz)
//
// a - Roughness
// h - Half vector
// n - Normal
// 
// D(h, n) = a^2 / pi * ((n dot h)^2 * (a^2 - 1) + 1)^2
float SpecDistribution(float3 n, float3 h, float roughness)
{
	// Pre-calculations
	float NdotH = saturate(dot(n, h));
	float NdotH2 = NdotH * NdotH;
	float a = roughness * roughness;
	float a2 = max(a * a, MIN_ROUGHNESS); // Applied after remap!

	// ((n dot h)^2 * (a^2 - 1) + 1)
	float denomToSquare = NdotH2 * (a2 - 1) + 1;
	// Can go to zero if roughness is 0 and NdotH is 1

	// Final value
	return a2 / (PI * denomToSquare * denomToSquare);
}



// Fresnel term - Schlick approx.
// 
// v - View vector
// h - Half vector
// f0 - Value when l = n (full specular color)
//
// F(v,h,f0) = f0 + (1-f0)(1 - (v dot h))^5
float3 Fresnel(float3 v, float3 h, float3 f0)
{
	// Pre-calculations
	float VdotH = saturate(dot(v, h));

	// Final value
	return f0 + (1 - f0) * pow(1 - VdotH, 5);
}


// Geometric Shadowing - Schlick-GGX (based on Schlick-Beckmann)
// - k is remapped to a / 2, roughness remapped to (r+1)/2
//
// n - Normal
// v - View vector
//
// G(l,v,h)
float GeometricShadowing(float3 n, float3 v, float3 h, float roughness)
{
	// End result of remapping:
	float k = pow(roughness + 1, 2) / 8.0f;
	float NdotV = saturate(dot(n, v));

	// Final value
	return NdotV / (NdotV * (1 - k) + k);
}



// Microfacet BRDF (Specular)
//
// f(l,v) = D(h)F(v,h)G(l,v,h) / 4(n dot l)(n dot v)
// - part of the denominator are canceled out by numerator (see below)
//
// D() - Spec Dist - Trowbridge-Reitz (GGX)
// F() - Fresnel - Schlick approx
// G() - Geometric Shadowing - Schlick-GGX
float3 MicrofacetBRDF(float3 n, float3 l, float3 v, float roughness, float metalness, float3 specColor)
{
	// Other vectors
	float3 h = normalize(v + l);

	// Grab various functions
	float D = SpecDistribution(n, h, roughness);
	float3 F = Fresnel(v, h, specColor);
	float G = GeometricShadowing(n, v, h, roughness) * GeometricShadowing(n, l, h, roughness);

	// Final formula
	// Denominator dot products partially canceled by G()!
	// See page 16: http://blog.selfshadow.com/publications/s2012-shading-course/hoffman/s2012_pbs_physics_math_notes.pdf
	return (D * F * G) / (4 * max(dot(n, v), dot(n, l)));
}


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

// Post process
struct VertexToPixelPP
{
	// Data type	Name	Semantic
	float4 position		: SV_POSITION;
	float2 uv			: TEXCOORD0;
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
float3 CalcLight(VertexToPixel input, float3 cameraPos, 
	float3 ambientColor, float3 diffuseColor, float3 direction, float3 surfaceColor,
	float roughness, float metalness)
{
	// Lighting calculations
	// Normalize needed vectors
	float3 normal = normalize(input.normal);
	float3 dir = normalize(direction);

	// Figure out how lit the object can be
	float3 lightAmount = saturate(dot(normal, dir));

	// Ambient
	float3 lightColor = float3(0,0,0);

	// Specularity vectors
	float3 V = normalize(cameraPos - input.worldPos);

	// Specularity calculation
	float3 specularColor = lerp(F0_NON_METAL.rrr, surfaceColor.rgb, metalness);
	
	// Specularity calculation
	float3 spec = MicrofacetBRDF(normal, dir, V, roughness, metalness, specularColor);

	// Conservation of energy
	float3 balancedDiff = DiffuseEnergyConserve(lightAmount, spec, metalness);

	// Finishing conservation of energy
	lightColor += balancedDiff * surfaceColor + spec;

	// Return the light created by this light specifically
	return lightColor;
};

float3 CalcLight(VertexToPixelNormals input, float3 cameraPos,
	float3 ambientColor, float3 diffuseColor, float3 direction, float3 surfaceColor,
	float roughness, float metalness)
{
	// Lighting calculations
	// Normalize needed vectors
	float3 normal = normalize(input.normal);
	float3 dir = normalize(direction);

	// Figure out how lit the object can be
	float3 lightAmount = saturate(dot(normal, dir));
	
	// Ambient
	float3 ambient = ambientColor;
	float3 lightColor = float3(0, 0, 0);

	// Specularity vectors
	float3 V = normalize(cameraPos - input.worldPos);

	// Specular
	float3 specularColor = lerp(F0_NON_METAL.rrr, surfaceColor.rgb, metalness);
	// Specularity calculation
	float3 spec = MicrofacetBRDF(normal, dir, V, roughness, metalness, specularColor);

	// Conservation of energy
	float3 balancedDiff = DiffuseEnergyConserve(lightAmount, spec, metalness);

	// Finishing conservation of energy
	lightColor += balancedDiff * surfaceColor + spec;
		
	// Return the light created by this light specifically
	return lightColor;
};

float3 CalcLight(VertexToPixelNormals input, float3 cameraPos,
	float3 ambientColor, float3 diffuseColor, float3 direction, float3 surfaceColor,
	float roughness, float metalness,
	Texture2D shadowChart, SamplerState samplerOptions)
{
	// Lighting calculations
	// Normalize needed vectors
	float3 normal = normalize(input.normal);
	float3 dir = normalize(direction);

	// Figure out how lit the object can be
	float3 lightAmount = saturate(dot(normal, dir));

	// Ambient
	float3 ambient = ambientColor;
	float3 lightColor = float3(0, 0, 0);

	// Specularity vectors
	float3 V = normalize(cameraPos - input.worldPos);

	// Specular
	float3 specularColor = lerp(F0_NON_METAL.rrr, surfaceColor.rgb, metalness);
	// Specularity calculation
	float3 spec = MicrofacetBRDF(normal, dir, V, roughness, metalness, specularColor);

	// Conservation of energy
	float3 balancedDiff = DiffuseEnergyConserve(lightAmount, spec, metalness);

	// Cel shaded versions of spec and balanced diff
	balancedDiff = shadowChart.Sample(samplerOptions, float2(balancedDiff.r, .5f)).rrr;
	spec = shadowChart.Sample(samplerOptions, float2(spec.r, 1)).rrr;

	// Finishing conservation of energy
	lightColor += balancedDiff * surfaceColor + spec;


	// Return the light created by this light specifically
	return lightColor;
};


#endif