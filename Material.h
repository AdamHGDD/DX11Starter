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
	std::shared_ptr<SimplePixelShader> pixelShader;
	std::shared_ptr<SimpleVertexShader> vertexShader;
public:
	// Constructor
	Material(XMFLOAT4 colorTint, std::shared_ptr<SimplePixelShader> pixelShader, std::shared_ptr<SimpleVertexShader> vertexShader);
	// Getters
	XMFLOAT4 GetColorTint();
	std::shared_ptr<SimplePixelShader> GetPixelShader();
	std::shared_ptr<SimpleVertexShader> GetVertexShader();
};

