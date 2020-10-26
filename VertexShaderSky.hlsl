#include "ShaderIncludes.hlsli"

// Constant buffer
cbuffer ExternalData : register(b0)
{
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
VertexToPixelSky main(VertexShaderInput input)
{
	// Set up output struct
	VertexToPixelSky output;

	// Remove translation
	matrix viewNoTranslation = viewMatrix;
	viewNoTranslation._14 = 0;
	viewNoTranslation._24 = 0;
	viewNoTranslation._34 = 0;

	// Multiply the world matrix by the view and then the projection matrix
	matrix vp = mul(projMatrix, viewNoTranslation);
	output.position = mul(vp, float4(input.position, 1.0f));
	output.position.z = output.position.w;

	// Direction just for this shader
	output.sampleDir = input.position;

	// Whatever we return will make its way through the pipeline to the
	// next programmable stage we're using (the pixel shader for now)
	return output;
}