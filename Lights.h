#include <DirectXMath.h>
#pragma once

struct DirectionalLight
{
	// Low level light that defines the color of the darkest spot
	DirectX::XMFLOAT3 ambientColor;
	// Fake variable to deal with buffer
	float padding1;
	// Color of the normal light
	DirectX::XMFLOAT3 diffuseColor;
	// Fake variable to deal with buffer
	float padding2;
	// Direction of the light
	DirectX::XMFLOAT3 direction;
};