#pragma once
#include <DirectXMath.h>
#include <wrl/client.h> 
#include "DXCore.h"
#include "SimpleShader.h"
#include <memory>

using namespace DirectX;

class Material
{
private:
	XMFLOAT4 colorTint;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> metalSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normalsSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> roughSRV;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
	float specularExponent;
	std::shared_ptr<SimplePixelShader> pixelShader;
	std::shared_ptr<SimpleVertexShader> vertexShader;
public:
	// Constructor with a lot of params
	Material(XMFLOAT4 colorTint, float specularExponent,
		std::shared_ptr<SimplePixelShader> pixelShader, std::shared_ptr<SimpleVertexShader> vertexShader,
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureSRV, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> metalSRV,
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normalsSRV, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> roughSRV,
		Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState);

	// Getters
	XMFLOAT4 GetColorTint();
	float GetSpecularExponent();
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetDiffuseSRV();
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetMetalSRV();
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetNormalsSRV();
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetRoughSRV();
	Microsoft::WRL::ComPtr<ID3D11SamplerState> GetSamplerState();
	void SetPixelShader(std::shared_ptr<SimplePixelShader> value);
	std::shared_ptr<SimplePixelShader> GetPixelShader();
	std::shared_ptr<SimpleVertexShader> GetVertexShader();
};

