#pragma once
#include <DirectXMath.h>
#include <wrl/client.h> 
#include "DXCore.h"
#include "SimpleShader.h"
#include <memory>
#include "Mesh.h"
#include "Camera.h"

class Sky
{
public:
	// Comptrs
	// Sampler options for the shader
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerOptions;
	// Texture map resource
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cubeTexSRV;
	// For adjusting the depth buffer comparison type
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
	// For switching to inside out
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerOptions;

	// Normal pointers
	// Mesh
	std::shared_ptr<Mesh> geometry;
	// Will hold the PixelShaderSky
	std::shared_ptr<SimplePixelShader> pixelShader;
	// Will hold the VertexShaderSky
	std::shared_ptr<SimpleVertexShader> vertexShader;

	// Constructor
	Sky(std::shared_ptr<Mesh> mesh, Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState,
		Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cubeTexSRV,
		std::shared_ptr<SimplePixelShader> pixelShader, std::shared_ptr<SimpleVertexShader> vertexShader);
	// Draw method
	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, std::shared_ptr<Camera> camera);
};

