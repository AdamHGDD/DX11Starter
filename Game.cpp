#include "Game.h"
#include "Vertex.h"
#include "BufferStructs.h"
#include "WICTextureLoader.h"

// Needed for a helper function to read compiled shader files from the hard drive
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>

// For the DirectX Math library
using namespace DirectX;

// --------------------------------------------------------
// Constructor
//
// DXCore (base class) constructor will set up underlying fields.
// DirectX itself, and our window, are not ready yet!
//
// hInstance - the application's OS-level handle (unique ID)
// --------------------------------------------------------
Game::Game(HINSTANCE hInstance)
	: DXCore(
		hInstance,		   // The application's handle
		"DirectX Game",	   // Text for the window's title bar
		1280,			   // Width of the window's client area
		720,			   // Height of the window's client area
		true)			   // Show extra stats (fps) in title bar?
{

#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateConsoleWindow(500, 120, 32, 120);
	printf("Console window created successfully.  Feel free to printf() here.\n");
#endif

}

// --------------------------------------------------------
// Destructor - Clean up anything our game has created:
//  - Release all DirectX objects created here
//  - Delete any objects to prevent memory leaks
// --------------------------------------------------------
Game::~Game()
{
	// Note: Since we're using smart pointers (ComPtr),
	// we don't need to explicitly clean up those DirectX objects
	// - If we weren't using smart pointers, we'd need
	//   to call Release() on each DirectX object created in Game

}

// --------------------------------------------------------
// Called once per program, after DirectX and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init()
{
	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	LoadShaders();
	CreateBasicGeometry();

	// Create sampler state description
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create sampler state
	device->CreateSamplerState(&samplerDesc, samplerState.GetAddressOf());

	// Textures using a method from  "WICTextureLoader.h" which is in "directxtk_desktop_win10" from NUGET
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../Assets/Textures/mossyStone.png").c_str(), nullptr, texture1SRV.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../Assets/Textures/woodPlanks.png").c_str(), nullptr, texture2SRV.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../Assets/Textures/cliff.png").c_str(), nullptr, texture3SRV.GetAddressOf());

	// Create the materials
	materials.push_back(std::shared_ptr<Material>(new Material(XMFLOAT4(1, 1, 1, 0), 16, pixelShader, vertexShader, texture1SRV, samplerState)));
	materials.push_back(std::shared_ptr<Material>(new Material(XMFLOAT4(.5, 1, 1, 0), 64, pixelShader, vertexShader, texture2SRV, samplerState)));
	materials.push_back(std::shared_ptr<Material>(new Material(XMFLOAT4(1, .5, 1, 0), 256, pixelShader, vertexShader, texture3SRV, samplerState)));

	// Create the game entities
	for (int i = 0; i < meshes.size(); i++)
	{
		// Create object
		entities.push_back(std::shared_ptr<GameEntity>(new GameEntity(meshes[i], materials[i/2])));
		// Move object away from others
		entities[i]->GetTransform()->LocalTranslate(i * 2 - 5, 0, 0);
	}

	// Get size as the next multiple of 16(don’t hardcode a numberhere!)
	unsigned int size = sizeof(VertexShaderExternalData);
	// Round up to the nearest 16
	size = (size + 15) / 16 * 16;
	// Create CBuffer Desc
	D3D11_BUFFER_DESC cbDesc = {};
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.ByteWidth = size;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	// Attach CBuffer
	// device->CreateBuffer(&cbDesc, 0, constantBufferVS.GetAddressOf());
	
	// Tell the input assembler stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.  
	// Essentially: "What kind of shape should the GPU draw with our data?"
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Add the camera away from the objects
	camera = std::shared_ptr<Camera> (new Camera(XMFLOAT3(0, 0, -20), XMFLOAT3(0, 0, 0), ((float)this->width / this->height), .5f, 1, 1000, 3, 3));

	// Set up the original directional light
	dLight.ambientColor = XMFLOAT3(0.01f, 0.01f, 0.1f);
	dLight.diffuseColor = XMFLOAT3(1.0f, 0.1f, 0.1f);
	dLight.direction = XMFLOAT3(0, -1, 0);

	// Set up the third directional light
	dLight2.ambientColor = XMFLOAT3(0.01f, 0.03f, 0.01f);
	dLight2.diffuseColor = XMFLOAT3(0.0f, 0.2f, 1);
	dLight2.direction = XMFLOAT3(0, 1, 0);

	// Set up the point light
	pLight.ambientColor = XMFLOAT3(0.01f, 0.03f, 0.01f);
	pLight.diffuseColor = XMFLOAT3(0.0f, 1, 0);
	pLight.position = XMFLOAT3(0, 0, 0);
}

// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files
// and also created the Input Layout that describes our 
// vertex data to the rendering pipeline. 
// - Input Layout creation is done here because it must 
//    be verified against vertex shader byte code
// - We'll have that byte code already loaded below
// --------------------------------------------------------
void Game::LoadShaders()
{
	// Create the vertex shader through simple shader
	vertexShader = std::shared_ptr<SimpleVertexShader> (new SimpleVertexShader(device.Get(), context.Get(), GetFullPathTo_Wide(L"VertexShader.cso").c_str()));
	// Create the pixel shader through simple shader
	pixelShader = std::shared_ptr<SimplePixelShader> (new SimplePixelShader(device.Get(), context.Get(), GetFullPathTo_Wide(L"PixelShader.cso").c_str()));
}



// --------------------------------------------------------
// Creates the geometry we're going to draw - a single triangle for now
// --------------------------------------------------------
void Game::CreateBasicGeometry()
{
	meshes.push_back(std::shared_ptr<Mesh>(new Mesh(GetFullPathTo("../Assets/Models/sphere.obj").c_str(), device)));
	meshes.push_back(std::shared_ptr<Mesh>(new Mesh(GetFullPathTo("../Assets/Models/cone.obj").c_str(), device)));
	meshes.push_back(std::shared_ptr<Mesh>(new Mesh(GetFullPathTo("../Assets/Models/cube.obj").c_str(), device)));
	meshes.push_back(std::shared_ptr<Mesh>(new Mesh(GetFullPathTo("../Assets/Models/cylinder.obj").c_str(), device)));
	meshes.push_back(std::shared_ptr<Mesh>(new Mesh(GetFullPathTo("../Assets/Models/helix.obj").c_str(), device)));
	meshes.push_back(std::shared_ptr<Mesh>(new Mesh(GetFullPathTo("../Assets/Models/torus.obj").c_str(), device)));

}


// --------------------------------------------------------
// Handle resizing DirectX "stuff" to match the new window size.
// For instance, updating our projection matrix's aspect ratio.
// --------------------------------------------------------
void Game::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();
	// Check for camera
	if (camera != nullptr)
	{
		// Update aspect ratio
		camera->UpdateProjectionMatrix((float)this->width / this->height);
	}
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	// Update the tranformations of the entities
	for (size_t i = 0; i < entities.size(); i++)
	{
		// Rotate the current entity
		entities[i]->GetTransform()->Rotate(0, 0, deltaTime);
	}

	// Update camera
	camera->Update(deltaTime, this->hWnd);

	// Quit if the escape key is pressed
	if (GetAsyncKeyState(VK_ESCAPE))
		Quit();
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Background color (Cornflower Blue in this case) for clearing
	const float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f };

	// Clear the render target and depth buffer (erases what's on the screen)
	//  - Do this ONCE PER FRAME
	//  - At the beginning of Draw (before drawing *anything*)
	context->ClearRenderTargetView(backBufferRTV.Get(), color);
	context->ClearDepthStencilView(
		depthStencilView.Get(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);

	// Send in lights to the shader
	pixelShader->SetData("dLight", &dLight, sizeof(DirectionalLight));
	pixelShader->SetData("dLight2", &dLight2, sizeof(DirectionalLight));
	pixelShader->SetData("pLight", &pLight, sizeof(PointLight));
	pixelShader->SetFloat3("cameraPos", camera->transform.GetPosition());
	pixelShader->CopyAllBufferData();
	
	// Draw all game entities
	for (size_t i = 0; i < entities.size(); i++)
	{
		pixelShader->SetFloat("specExponent", entities[i]->material->GetSpecularExponent());
		pixelShader->SetShaderResourceView("diffuseTexture", entities[i]->material->GetSRV().Get());
		pixelShader->SetSamplerState("samplerOptions", samplerState.Get());
		pixelShader->CopyAllBufferData();
		entities[i]->Draw(context, camera);
	}


	// Present the back buffer to the user
	//  - Puts the final frame we're drawing into the window so the user can see it
	//  - Do this exactly ONCE PER FRAME (always at the very end of the frame)
	swapChain->Present(0, 0);

	// Due to the usage of a more sophisticated swap chain,
	// the render target must be re-bound after every call to Present()
	context->OMSetRenderTargets(1, backBufferRTV.GetAddressOf(), depthStencilView.Get());
}