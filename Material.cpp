#include "Material.h"

Material::Material(XMFLOAT4 colorTint, float specularExponent, std::shared_ptr<SimplePixelShader> pixelShader, std::shared_ptr<SimpleVertexShader> vertexShader, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureSRV, Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState)
{
	// Set values
	this->colorTint = colorTint;
	this->specularExponent = specularExponent;
	this->vertexShader = vertexShader;
	this->pixelShader = pixelShader;
	this->textureSRV = textureSRV;
	this->samplerState = samplerState;
}

// Simple getters
XMFLOAT4 Material::GetColorTint() { return colorTint; }
float Material::GetSpecularExponent() { return specularExponent; }
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Material::GetSRV() { return textureSRV; }
Microsoft::WRL::ComPtr<ID3D11SamplerState> Material::GetSamplerState() { return samplerState; }
std::shared_ptr<SimplePixelShader> Material::GetPixelShader() { return pixelShader; }
std::shared_ptr<SimpleVertexShader> Material::GetVertexShader() { return vertexShader; }
