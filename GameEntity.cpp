#include "GameEntity.h"

// Constructor that brings in the mesh
GameEntity::GameEntity(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material)
{
	// Assign mesh
	this->mesh = mesh;
	// Assign material
	this->material = material;
}

// Return mesh pointer
std::shared_ptr<Mesh> GameEntity::GetMesh()
{
	return mesh;
}

// Return a pointer to the transform
Transform* GameEntity::GetTransform()
{
	return &transform;
}

void GameEntity::Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, Microsoft::WRL::ComPtr<ID3D11Buffer> constantBufferVS, std::shared_ptr<Camera> camera)
{
	// Set the vertex and pixel shaders to use for the next Draw() command
	//  - These don't technically need to be set every frame
	//  - Once you start applying different shaders to different objects,
	//    you'll need to swap the current shaders before each draw
	context->VSSetShader(material->GetVertexShader().Get(), 0, 0);
	context->PSSetShader(material->GetPixelShader().Get(), 0, 0);

	// Constant Buffer defined data
	VertexShaderExternalData vsData;
	vsData.colorTint = material->GetColorTint();
	vsData.worldMatrix = transform.GetWorldMatrix();
	// Get camera information and put it in the cbuffer
	vsData.viewMatrix = camera->GetViewMatrix();
	vsData.projMatrix = camera->GetProjMatrix();

	// Create a buffer for the memoty
	D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
	// Lock the GPU off
	context->Map(constantBufferVS.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
	// Copy over data
	memcpy(mappedBuffer.pData, &vsData, sizeof(vsData));
	// Unlock the GPU
	context->Unmap(constantBufferVS.Get(), 0);

	// Bind the Constant buffer
	context->VSSetConstantBuffers(0, 1, constantBufferVS.GetAddressOf());


	// Set buffers in the input assembler
		//  - Do this ONCE PER OBJECT you're drawing, since each object might
		//    have different geometry.
		//  - for this demo, this step *could* simply be done once during Init(),
		//    but I'm doing it here because it's often done multiple times per frame
		//    in a larger application/game
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, mesh->GetVertexBuffer().GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(mesh->GetIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);


	// Finally do the actual drawing
	//  - Do this ONCE PER OBJECT you intend to draw
	//  - This will use all of the currently set DirectX "stuff" (shaders, buffers, etc)
	//  - DrawIndexed() uses the currently set INDEX BUFFER to look up corresponding
	//     vertices in the currently set VERTEX BUFFER
	context->DrawIndexed(
		mesh->GetIndexCount(),     // The number of indices to use (we could draw a subset if we wanted)
		0,     // Offset to the first index we want to use
		0);    // Offset to add to each index when looking up vertices
}
