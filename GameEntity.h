#pragma once
#include "Transform.h"
#include "Camera.h"
#include "Mesh.h"
#include "Vertex.h"
#include "BufferStructs.h"
#include <d3dcompiler.h>
#include <memory>
#include "Material.h"

class GameEntity
{
public:
	// Constructor that brings in mesh
	GameEntity(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material);

	// Two fields
	// Tranformation data (position, rotation, scale)
	Transform transform;
	// Model
	std::shared_ptr<Mesh> mesh;
	// Material
	std::shared_ptr<Material> material;

	// Getters
	std::shared_ptr<Mesh> GetMesh();
	Transform* GetTransform();

	// Draw call
	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, Microsoft::WRL::ComPtr<ID3D11Buffer> constantBufferVS, std::shared_ptr<Camera> camera);
};

