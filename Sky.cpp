#include "Sky.h"
#include "WICTextureLoader.h"

using namespace DirectX;

Sky::Sky(std::shared_ptr<Mesh> mesh, Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState,
	Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cubeTexSRV,
	std::shared_ptr<SimplePixelShader> pixelShader, std::shared_ptr<SimpleVertexShader> vertexShader)
{
	// Attach objects
	geometry = mesh;
	samplerOptions = samplerState;
	this->pixelShader = pixelShader;
	this->vertexShader = vertexShader;
	this->cubeTexSRV = cubeTexSRV;

	// Set up states
	// First depth stencil state
	D3D11_DEPTH_STENCIL_DESC stencilDesc = {};
	stencilDesc.DepthEnable = true;
	// Allows 1.0 dist to be valid
	stencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	device->CreateDepthStencilState(&stencilDesc, depthStencilState.GetAddressOf());
	// Next set up rasterizer
	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	// Reverse faces
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_FRONT;
	device->CreateRasterizerState(&rasterizerDesc, rasterizerOptions.GetAddressOf());
}


void Sky::Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, std::shared_ptr<Camera> camera)
{
	// Change states for rendering
	context->RSSetState(rasterizerOptions.Get());
	context->OMSetDepthStencilState(depthStencilState.Get(), 0);

	// Prepare the sky specific shaders
	vertexShader->SetShader();
	pixelShader->SetShader();
	// Set vertex shader data
	vertexShader->SetMatrix4x4("viewMatrix", camera->GetViewMatrix());
	vertexShader->SetMatrix4x4("projMatrix", camera->GetProjMatrix());
	// Set pixel shader data
	pixelShader->SetSamplerState("samplerOptions", samplerOptions.Get());
	pixelShader->SetShaderResourceView("cubeTex", cubeTexSRV.Get());
	// Send data in
	vertexShader->CopyAllBufferData();
	pixelShader->CopyAllBufferData();

	// Set up drawing the mesh
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, geometry->GetVertexBuffer().GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(geometry->GetIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);

	// Draw the mesh
	context->DrawIndexed(
		geometry->GetIndexCount(),     // The number of indices to use (we could draw a subset if we wanted)
		0,     // Offset to the first index we want to use
		0);    // Offset to add to each index when looking up vertices

	// Reset states
	context->RSSetState(nullptr);
	context->OMSetDepthStencilState(nullptr, 0);
}