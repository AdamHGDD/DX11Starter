#pragma once

#include "DXCore.h"
#include "Mesh.h"
#include "GameEntity.h"
#include "Lights.h"
#include "Camera.h"
#include "Sky.h"
#include <DirectXMath.h>
#include <wrl/client.h> // Used for ComPtr - a smart pointer for COM objects
#include <vector>
#include <memory>

class Game 
	: public DXCore
{

public:
	Game(HINSTANCE hInstance);
	~Game();

	// Overridden setup and game loop methods, which
	// will be called automatically
	void Init();
	void OnResize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);

private:

	// Initialization helper methods - feel free to customize, combine, etc.
	void LoadShaders(); 
	void CreateBasicGeometry();

	
	// Note the usage of ComPtr below
	//  - This is a smart pointer for objects that abide by the
	//    Component Object Model, which DirectX objects do
	//  - More info here: https://github.com/Microsoft/DirectXTK/wiki/ComPtr

	// Buffers to hold actual geometry data
	// Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	// Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	
	// Shaders now done with simple shader
	std::shared_ptr<SimplePixelShader> pixelShader;
	std::shared_ptr<SimpleVertexShader> vertexShader;
	std::shared_ptr<SimplePixelShader> pixelShaderNormals;
	std::shared_ptr<SimplePixelShader> pixelToon;
	std::shared_ptr<SimpleVertexShader> vertexShaderNormals;
	std::shared_ptr<SimplePixelShader> pixelShaderSky;
	std::shared_ptr<SimpleVertexShader> vertexShaderSky;

	// CBuffer
	//	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBufferVS;

	std::vector<std::shared_ptr<Mesh>> meshes;
	std::vector<std::shared_ptr<GameEntity>> entities;
	std::vector<std::shared_ptr<Material>> materials;
	std::shared_ptr<Camera> camera;

	// Textures
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture1SRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture2SRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture3SRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture4SRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture5SRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture6SRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture7SRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture8SRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture9SRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture10SRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture11SRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture12SRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shadowSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cubeTexSRV;

	// Sampler state for textures
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState2;
	D3D11_SAMPLER_DESC samplerDesc;

	DirectionalLight dLight;
	PointLight pLight;

	// Skybox
	std::shared_ptr<Sky> sky;
};

