#pragma once
#include "Vertex.h"
#include <DirectXMath.h>
#include <wrl/client.h>
#include <Windows.h>
#include <d3d11.h>
#include <string>

class Mesh
{
private:
	// Buffer objects
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	// Number of indices
	int indexCount;

public:
	// Methods for getting information
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer();
	int GetIndexCount();

	// Constructor
	Mesh(Vertex* vertexList, int vertexCount, UINT* indexList, int indexCount, Microsoft::WRL::ComPtr<ID3D11Device> device);
};

