#include "Material.h"

Material::Material(XMFLOAT4 colorTint, std::shared_ptr<SimplePixelShader> pixelShader, std::shared_ptr<SimpleVertexShader> vertexShader)
{
	// Set values
	this->colorTint = colorTint;
	this->vertexShader = vertexShader;
	this->pixelShader = pixelShader;
}

// Simple getters
XMFLOAT4 Material::GetColorTint() { return colorTint; }
std::shared_ptr<SimplePixelShader> Material::GetPixelShader() { return pixelShader; }
std::shared_ptr<SimpleVertexShader> Material::GetVertexShader() { return vertexShader; }
