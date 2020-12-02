#include "Game.h"
#include "Vertex.h"
#include "BufferStructs.h"
#include "WICTextureLoader.h"
#include "DDSTextureLoader.h"

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

// Not original code, came from your code
void Game::ResizePostProcessResources()
{
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.ArraySize = 1;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE; // Will render to it and sample from it!
	textureDesc.CPUAccessFlags = 0;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.MipLevels = 1;
	textureDesc.MiscFlags = 0;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;

	// Create the color and normals textures
	ID3D11Texture2D* ppTexture;

	device->CreateTexture2D(&textureDesc, 0, &ppTexture);

	// Again for shadows
	ID3D11Texture2D* sceneShadowsTexture;
	device->CreateTexture2D(&textureDesc, 0, &sceneShadowsTexture);


	// Adjust the description for scene normals
	textureDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	ID3D11Texture2D* sceneNormalsTexture;
	device->CreateTexture2D(&textureDesc, 0, &sceneNormalsTexture);

	// Adjust the description for the scene depths
	textureDesc.Format = DXGI_FORMAT_R32_FLOAT;
	ID3D11Texture2D* sceneDepthsTexture;
	device->CreateTexture2D(&textureDesc, 0, &sceneDepthsTexture);

	// Create the Render Target Views
	device->CreateRenderTargetView(ppTexture, 0, ppRTV.ReleaseAndGetAddressOf());
	device->CreateRenderTargetView(sceneNormalsTexture, 0, sceneNormalsRTV.ReleaseAndGetAddressOf());
	device->CreateRenderTargetView(sceneDepthsTexture, 0, sceneDepthRTV.ReleaseAndGetAddressOf());
	device->CreateRenderTargetView(sceneShadowsTexture, 0, sceneShadowsRTV.ReleaseAndGetAddressOf());

	// Create the Shader Resource Views
	device->CreateShaderResourceView(ppTexture, 0, ppSRV.ReleaseAndGetAddressOf());
	device->CreateShaderResourceView(sceneNormalsTexture, 0, sceneNormalsSRV.ReleaseAndGetAddressOf());
	device->CreateShaderResourceView(sceneDepthsTexture, 0, sceneDepthSRV.ReleaseAndGetAddressOf());
	device->CreateShaderResourceView(sceneShadowsTexture, 0, sceneShadowsSRV.ReleaseAndGetAddressOf());

	// Release the extra texture references
	ppTexture->Release();
	sceneNormalsTexture->Release();
	sceneDepthsTexture->Release();
	sceneShadowsTexture->Release();
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
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.MinLOD = 1;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create sampler state
	device->CreateSamplerState(&samplerDesc, samplerState.GetAddressOf());

	// Create sampler state description 2
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.MinLOD = 1;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	device->CreateSamplerState(&samplerDesc, samplerState2.GetAddressOf());

	// Textures using a method from  "WICTextureLoader.h" which is in "directxtk_desktop_win10" from NUGET
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/wood_albedo.png").c_str(), nullptr, texture1SRV.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/wood_metal.png").c_str(), nullptr, texture2SRV.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/wood_normals.png").c_str(), nullptr, texture3SRV.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/wood_roughness.png").c_str(), nullptr, texture4SRV.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/paint_albedo.png").c_str(), nullptr, texture5SRV.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/paint_metal.png").c_str(), nullptr, texture6SRV.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/paint_normals.png").c_str(), nullptr, texture7SRV.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/paint_roughness.png").c_str(), nullptr, texture8SRV.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/scratched_albedo.png").c_str(), nullptr, texture9SRV.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/scratched_metal.png").c_str(), nullptr, texture10SRV.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/scratched_normals.png").c_str(), nullptr, texture11SRV.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/scratched_roughness.png").c_str(), nullptr, texture12SRV.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/cobblestone_albedo.png").c_str(), nullptr, texture13SRV.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/cobblestone_metal.png").c_str(), nullptr, texture14SRV.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/cobblestone_normals.png").c_str(), nullptr, texture15SRV.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/cobblestone_roughness.png").c_str(), nullptr, texture16SRV.GetAddressOf());

	// Sky texture
	CreateDDSTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/SunnyCubeMap.dds").c_str(), nullptr, cubeTexSRV.GetAddressOf());
	
	// Cel texture
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/celChart.png").c_str(), nullptr, shadowSRV.GetAddressOf());

	// Stipple texture
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/stipple.png").c_str(), nullptr, stippleSRV.GetAddressOf());

	// Create the materials
	materials.push_back(std::shared_ptr<Material>(new Material(XMFLOAT4(1, 1, 1, 0), 16, pixelShaderNormals, vertexShaderNormals, texture1SRV, texture2SRV, texture3SRV, texture4SRV, samplerState)));
	materials.push_back(std::shared_ptr<Material>(new Material(XMFLOAT4(1, 1, 1, 0), 16, pixelShaderNormals, vertexShaderNormals, texture1SRV, texture2SRV, texture3SRV, texture4SRV, samplerState)));
	materials.push_back(std::shared_ptr<Material>(new Material(XMFLOAT4(.5, 1, 1, 0), 64, pixelShaderNormals, vertexShaderNormals, texture5SRV, texture6SRV, texture7SRV, texture8SRV, samplerState)));
	materials.push_back(std::shared_ptr<Material>(new Material(XMFLOAT4(.5, 1, 1, 0), 64, pixelShaderNormals, vertexShaderNormals, texture5SRV, texture6SRV, texture7SRV, texture8SRV, samplerState)));
	materials.push_back(std::shared_ptr<Material>(new Material(XMFLOAT4(1, .5, 1, 0), 256, pixelShaderNormals, vertexShaderNormals, texture9SRV, texture10SRV, texture11SRV, texture12SRV, samplerState)));
	materials.push_back(std::shared_ptr<Material>(new Material(XMFLOAT4(1, .5, 1, 0), 256, pixelShaderNormals, vertexShaderNormals, texture9SRV, texture10SRV, texture11SRV, texture12SRV, samplerState)));
	
	// Create the sky
	sky = std::shared_ptr<Sky>(new Sky(meshes[2], samplerState, device, cubeTexSRV, pixelShaderSky, vertexShaderSky));

	// Create the game entities
	for (int i = 0; i < meshes.size(); i++)
	{
		// Create object
		entities.push_back(std::shared_ptr<GameEntity>(new GameEntity(meshes[i], materials[i])));
		// Move object away from others
		entities[i]->GetTransform()->LocalTranslate(i * 2.0f - 5.0f, 0, 0);
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
	dLight.diffuseColor = XMFLOAT3(1.0f, 1.0f, 1.0f);
	dLight.direction = XMFLOAT3(0, -1, -.2f);

	// Set up the point light
	pLight.ambientColor = XMFLOAT3(0.01f, 0.03f, 0.01f);
	pLight.diffuseColor = XMFLOAT3(1.0f, 1.0f, 1.0f);
	pLight.position = XMFLOAT3(0, 0, 0);

	// Set up post process stuff
	ResizePostProcessResources();
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
	vertexShader = std::shared_ptr<SimpleVertexShader>(new SimpleVertexShader(device.Get(), context.Get(), GetFullPathTo_Wide(L"VertexShader.cso").c_str()));
	// Create the pixel shader through simple shader
	pixelShader = std::shared_ptr<SimplePixelShader>(new SimplePixelShader(device.Get(), context.Get(), GetFullPathTo_Wide(L"PixelShader.cso").c_str()));

	// New vertex shader that has normals
	vertexShaderNormals = std::shared_ptr<SimpleVertexShader>(new SimpleVertexShader(device.Get(), context.Get(), GetFullPathTo_Wide(L"VertexShaderNormals.cso").c_str()));
	// New pixel shader that has normals
	pixelShaderNormals = std::shared_ptr<SimplePixelShader>(new SimplePixelShader(device.Get(), context.Get(), GetFullPathTo_Wide(L"PixelShaderNormals.cso").c_str()));

	// Special pixel shaders
	// New pixel shader that has normals and is toone
	pixelToon = std::shared_ptr<SimplePixelShader>(new SimplePixelShader(device.Get(), context.Get(), GetFullPathTo_Wide(L"PixelShaderToonNormals.cso").c_str()));
	
	// Post processing shaders
	postProcessVS = std::shared_ptr<SimpleVertexShader>(new SimpleVertexShader(device.Get(), context.Get(), GetFullPathTo_Wide(L"VertexShaderPP.cso").c_str()));
	pixelPostProcess = std::shared_ptr<SimplePixelShader>(new SimplePixelShader(device.Get(), context.Get(), GetFullPathTo_Wide(L"PixelShaderToonPostProcess.cso").c_str()));

	// Skybox specific vertex shader
	vertexShaderSky = std::shared_ptr<SimpleVertexShader>(new SimpleVertexShader(device.Get(), context.Get(), GetFullPathTo_Wide(L"VertexShaderSky.cso").c_str()));
	// Skybox specific pixel shader
	pixelShaderSky = std::shared_ptr<SimplePixelShader>(new SimplePixelShader(device.Get(), context.Get(), GetFullPathTo_Wide(L"PixelShaderSky.cso").c_str()));
}



// --------------------------------------------------------
// Creates the geometry we're going to draw - a single triangle for now
// --------------------------------------------------------
void Game::CreateBasicGeometry()
{
	meshes.push_back(std::shared_ptr<Mesh>(new Mesh(GetFullPathTo("../../Assets/Models/sphere.obj").c_str(), device)));
	meshes.push_back(std::shared_ptr<Mesh>(new Mesh(GetFullPathTo("../../Assets/Models/cone.obj").c_str(), device)));
	meshes.push_back(std::shared_ptr<Mesh>(new Mesh(GetFullPathTo("../../Assets/Models/cube.obj").c_str(), device)));
	meshes.push_back(std::shared_ptr<Mesh>(new Mesh(GetFullPathTo("../../Assets/Models/cylinder.obj").c_str(), device)));
	meshes.push_back(std::shared_ptr<Mesh>(new Mesh(GetFullPathTo("../../Assets/Models/helix.obj").c_str(), device)));
	meshes.push_back(std::shared_ptr<Mesh>(new Mesh(GetFullPathTo("../../Assets/Models/torus.obj").c_str(), device)));

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
	// Post process
	ResizePostProcessResources();
}


// Method for switching between post processing types
void Game::InputCheck()
{
	// Check for input to switch shaders
	if (GetAsyncKeyState('1') & 0x8000)
	{
		postProcessing = false;
		stipple = false;
		for (int i = 0; i < materials.size(); i++)
		{
			materials[i]->SetPixelShader(pixelShaderNormals);
		}
	}
	if (GetAsyncKeyState('2') & 0x8000)
	{
		// Enable toon shading
		postProcessing = true;
		stipple = false;
		// Set post processing shader to be toon shading
		pixelPostProcess = std::shared_ptr<SimplePixelShader>(new SimplePixelShader(device.Get(), context.Get(), GetFullPathTo_Wide(L"PixelShaderToonPostProcess.cso").c_str()));
		for (int i = 0; i < materials.size(); i++)
		{
			// Set shadow type
			materials[i]->SetPixelShader(pixelToon);
		}
	}
	if (GetAsyncKeyState('3') & 0x8000)
	{
		// Enable toon shading
		postProcessing = true;
		stipple = false;
		// Set post processing shader to be hatching shading
		pixelPostProcess = std::shared_ptr<SimplePixelShader>(new SimplePixelShader(device.Get(), context.Get(), GetFullPathTo_Wide(L"PixelShaderOutlinePostProcess.cso").c_str()));
		for (int i = 0; i < materials.size(); i++)
		{
			// Set shadow type
			materials[i]->SetPixelShader(pixelToon);
		}
	}
	if (GetAsyncKeyState('4') & 0x8000)
	{
		// Enable toon shading
		postProcessing = true;
		stipple = true;
		// Set post processing shader to be hatching shading
		pixelPostProcess = std::shared_ptr<SimplePixelShader>(new SimplePixelShader(device.Get(), context.Get(), GetFullPathTo_Wide(L"PixelShaderHatchingPostProcess.cso").c_str()));
		for (int i = 0; i < materials.size(); i++)
		{
			// Set shadow type
			materials[i]->SetPixelShader(pixelToon);
		}
	}
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	// Check for input to switch shaders
	InputCheck();

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
		D3D11_CLEAR_DEPTH,
		1.0f,
		0);

	// Set up post processing rendering
	if (postProcessing)
	{
		context->ClearRenderTargetView(ppRTV.Get(), color);
		context->ClearRenderTargetView(sceneNormalsRTV.Get(), color);
		context->ClearRenderTargetView(sceneDepthRTV.Get(), color);
		context->ClearRenderTargetView(sceneShadowsRTV.Get(), color);

		// Set post processing rendertargets
		ID3D11RenderTargetView* rtvs[4] =
		{
			ppRTV.Get(),
			sceneShadowsRTV.Get(),
			sceneNormalsRTV.Get(),
			sceneDepthRTV.Get()
		};
		context->OMSetRenderTargets(4, rtvs, depthStencilView.Get());
	}

	
	// Draw all game entities
	for (size_t i = 0; i < entities.size(); i++)
	{
		// Send in lights to the shader
		entities[i]->material->GetPixelShader()->SetData("dLight", &dLight, sizeof(DirectionalLight));
		entities[i]->material->GetPixelShader()->SetData("pLight", &pLight, sizeof(PointLight));
		entities[i]->material->GetPixelShader()->SetFloat3("cameraPos", camera->transform.GetPosition());
		entities[i]->material->GetPixelShader()->SetFloat("specExponent", entities[i]->material->GetSpecularExponent());
		// Send in textures
		entities[i]->material->GetPixelShader()->SetShaderResourceView("Albedo", entities[i]->material->GetDiffuseSRV().Get());
		entities[i]->material->GetPixelShader()->SetShaderResourceView("MetalnessMap", entities[i]->material->GetMetalSRV().Get());
		entities[i]->material->GetPixelShader()->SetShaderResourceView("RoughnessMap", entities[i]->material->GetRoughSRV().Get());
		entities[i]->material->GetPixelShader()->SetShaderResourceView("ShadowChart", shadowSRV.Get());
		// Check for if it has a normal
		if (entities[i]->material->GetNormalsSRV() != nullptr)
		{
			entities[i]->material->GetPixelShader()->SetShaderResourceView("NormalMap", entities[i]->material->GetNormalsSRV().Get());
		}
		entities[i]->material->GetPixelShader()->SetSamplerState("samplerOptions", samplerState.Get());
		entities[i]->material->GetPixelShader()->SetSamplerState("samplerState2", samplerState2.Get());
		// Send the data actually into the shader
		entities[i]->material->GetPixelShader()->CopyAllBufferData();
		entities[i]->Draw(context, camera);
	}

	// Draw the sky
	sky->Draw(context, camera);


	// Render post processing
	if (postProcessing)
	{
		// Now that the scene is rendered, swap to the back buffer
		context->OMSetRenderTargets(1, backBufferRTV.GetAddressOf(), 0);

		// Set up post process shaders
		postProcessVS->SetShader();

		pixelPostProcess->SetShaderResourceView("PixelsRender", ppSRV.Get());
		pixelPostProcess->SetShaderResourceView("NormalsRender", sceneNormalsSRV.Get());
		pixelPostProcess->SetShaderResourceView("DepthsRender", sceneDepthSRV.Get());
		pixelPostProcess->SetShaderResourceView("ShadowsRender", sceneShadowsSRV.Get());
		if (stipple)
		{
			pixelPostProcess->SetShaderResourceView("Stipple", stippleSRV.Get());
		}
		pixelPostProcess->SetSamplerState("samplerOptions", samplerState2.Get());
		pixelPostProcess->SetShader();

		// Send over window size data
		pixelPostProcess->SetFloat("pixelWidth", 1.0f / width);
		pixelPostProcess->SetFloat("pixelHeight", 1.0f / height);
		pixelPostProcess->SetFloat("depthAdjust", 5.0f);
		pixelPostProcess->SetFloat("normalAdjust", 5.0f);
		pixelPostProcess->CopyAllBufferData();

		// Turn OFF my vertex and index buffers
		context->IASetIndexBuffer(0, DXGI_FORMAT_R32_UINT, 0);
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		ID3D11Buffer* nothing = 0;
		context->IASetVertexBuffers(0, 1, &nothing, &stride, &offset);

		// Draw exactly 3 vertices, which the special post-process vertex shader will
		// "figure out" on the fly (resulting in our "full screen triangle")
		context->Draw(3, 0);
	}

	ID3D11ShaderResourceView* nullSRVs[16] = {};
	context->PSSetShaderResources(0, 16, nullSRVs);


	// Present the back buffer to the user
	//  - Puts the final frame we're drawing into the window so the user can see it
	//  - Do this exactly ONCE PER FRAME (always at the very end of the frame)
	swapChain->Present(0, 0);

	// Due to the usage of a more sophisticated swap chain,
	// the render target must be re-bound after every call to Present()
	context->OMSetRenderTargets(1, backBufferRTV.GetAddressOf(), depthStencilView.Get());

}