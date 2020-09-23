#include "Material.h"

Material::Material(XMFLOAT4 colorTint, Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader, Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader)
{
	// Set values
	this->colorTint = colorTint;
	this->vertexShader = vertexShader;
	this->pixelShader = pixelShader;
}

// Simple getters
XMFLOAT4 Material::GetColorTint() { return colorTint; }
Microsoft::WRL::ComPtr<ID3D11PixelShader> Material::GetPixelShader() { return pixelShader; }
Microsoft::WRL::ComPtr<ID3D11VertexShader> Material::GetVertexShader() { return vertexShader; }
