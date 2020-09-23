#pragma once
#include <DirectXMath.h>

// So I don't have to keep retyping DirectX:: before every vector
using namespace DirectX;

class Transform
{
private:
	// Fields
	// World matrix which contains all of the following transformations
	XMFLOAT4X4 worldMatrix;
	// Position
	XMFLOAT3 position;
	// Scale
	XMFLOAT3 scale;
	// Rotation
	XMFLOAT3 rotation;
	// Does the world matrix need to be recalculated
	bool dirty;

public:
	// Default constructor
	Transform();

	// Getter for world matrix
	XMFLOAT4X4 GetWorldMatrix();
	// Getter and Setter for position
	XMFLOAT3 GetPosition();
	void SetPosition(float x, float y, float z);
	// Getter and Setter for scale
	XMFLOAT3 GetScale();
	void SetScale(float x, float y, float z);
	// Getter and Setter for rotation
	XMFLOAT3 GetRotation();
	void SetRotation(float pitch, float yaw, float roll);

	// Transformations
	// This is the a rename for moveabsolute because it makes more sense to me
	void WorldTranslate(float x, float y, float z);
	// Method to implement in the future for moving depending on the forward vector
	void LocalTranslate(float x, float y, float z);
	// Multiply the scale
	void Scale(float x, float y, float z);
	// Add to the rotation
	void Rotate(float pitch, float yaw, float roll);
};

