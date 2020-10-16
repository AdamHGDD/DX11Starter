#include "ShaderIncludes.hlsli"

// Constant buffer
cbuffer ExternalData : register(b0)
{
	float4 colorTint;
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projMatrix;
}

// --------------------------------------------------------
// The entry point (main method) for our vertex shader
// 
// - Input is exactly one vertex worth of data (defined by a struct)
// - Output is a single struct of data to pass down the pipeline
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
VertexToPixelNormals main(VertexShaderInput input)
{
	// Set up output struct
	VertexToPixelNormals output;

	// Here we're essentially passing the input position directly through to the next
	// stage (rasterizer), though it needs to be a 4-component vector now.  
	// - To be considered within the bounds of the screen, the X and Y components 
	//   must be between -1 and 1.  
	// - The Z component must be between 0 and 1.  
	// - Each of these components is then automatically divided by the W component, 
	//   which we're leaving at 1.0 for now (this is more useful when dealing with 
	//   a perspective projection matrix, which we'll get to in the future).

	// Multiply the world matrix by the view and then the projection matrix
	matrix wvp = mul(projMatrix, mul(viewMatrix, worldMatrix));
	output.position = mul(wvp, float4(input.position, 1.0f));

	// World position of the point
	output.worldPos = mul(worldMatrix, float4(input.position, 1.0f)).xyz;

	// Pass the color through 
	// - The values will be interpolated per-pixel by the rasterizer
	// - We don't need to alter it here, but we do need to send it to the pixel shader
	output.color = colorTint;

	// Move the normal into world space (no translation)
	output.normal = mul((float3x3)worldMatrix, input.normal);

	// Get a normalized vector of the tangent rotated into world space
	output.tangent = normalize(mul((float3x3)worldMatrix, input.tangent));

	// Keep uvs the same
	output.uv = input.uv;

	// Whatever we return will make its way through the pipeline to the
	// next programmable stage we're using (the pixel shader for now)
	return output;
}