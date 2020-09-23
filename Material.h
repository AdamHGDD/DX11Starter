#pragma once
#include <DirectXMath.h>
#include <wrl/client.h> 
#include "DXCore.h"

using namespace DirectX;

class Material
{
private:
	XMFLOAT4 colorTint;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
public:
	// Constructor
	Material(XMFLOAT4 colorTint, Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader, Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader);
	// Getters
	XMFLOAT4 GetColorTint();
	Microsoft::WRL::ComPtr<ID3D11PixelShader> GetPixelShader();
	Microsoft::WRL::ComPtr<ID3D11VertexShader> GetVertexShader();
};

