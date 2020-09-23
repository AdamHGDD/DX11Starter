#include "Transform.h"

Transform::Transform()
{
	// Set default values
	position = XMFLOAT3(0, 0, 0);
	rotation = XMFLOAT3(0, 0, 0);
	scale = XMFLOAT3(1, 1, 1);
	// Values have been changed
	dirty = true;
	// Get default world matrix calculated
	XMStoreFloat4x4(&worldMatrix, XMMatrixIdentity());
}

// Return the world matrix
XMFLOAT4X4 Transform::GetWorldMatrix()
{
	// Check if current matrix is correct
	if (dirty)
	{
		// Create temp matrices out of the transform data
		XMMATRIX transMatrix = XMMatrixTranslation(position.x, position.y, position.z);
		XMMATRIX scaleMatrix = XMMatrixScaling(scale.x, scale.y, scale.z);
		// The name is order of when the rotations are done
		// The parameters are sill pitch yaw roll
		XMMATRIX rotMatrix = XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);

		// Create the matrix (reverse order)
		XMMATRIX world = scaleMatrix * rotMatrix * transMatrix;
		
		// Translate it back into xmfloat4x4
		XMStoreFloat4x4(&worldMatrix, world);
	}
	// Return the world matrix
	return worldMatrix;
}

// Return the position
XMFLOAT3 Transform::GetPosition()
{
	return position;
}

// Set the position directly
void Transform::SetPosition(float x, float y, float z)
{
	position = XMFLOAT3(x, y, z);
	// A piece of transformation data has changed
	dirty = true;
}

// Return the scale
XMFLOAT3 Transform::GetScale()
{
	return scale;
}

// Set the scale directly
void Transform::SetScale(float x, float y, float z)
{
	scale = XMFLOAT3(x, y, z);
	// A piece of transformation data has changed
	dirty = true;
}

// Return the rotation
XMFLOAT3 Transform::GetRotation()
{
	return rotation;
}

// Set the rotation directly
void Transform::SetRotation(float pitch, float yaw, float roll)
{
	rotation = XMFLOAT3(pitch, yaw, roll);
	// A piece of transformation data has changed
	dirty = true;
}


// Transform methods
// Add to the position
void Transform::WorldTranslate(float x, float y, float z)
{
	// Calculate the new position
	position = XMFLOAT3(position.x + x, position.y + y, position.z + z);
	// A piece of transformation data has changed
	dirty = true;
}

// Add to the position within a local relative space
void Transform::LocalTranslate(float x, float y, float z)
{
	// Move to information into the XMVECTOR for math 
	XMVECTOR vec = XMVectorSet(x, y, z, 0);
	XMVECTOR rotationVec = XMQuaternionRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
	// Rotate the vector and add it on
	XMVECTOR newPos = XMLoadFloat3(&position) + XMVector3Rotate(vec, rotationVec);
	// Move back from Vector to Float3
	XMStoreFloat3(&position, newPos);
}

// Multiply the scale
void Transform::Scale(float x, float y, float z)
{
	// Calculate the new scale
	scale = XMFLOAT3(scale.x * x, scale.y * y, scale.z * z);
	// A piece of transformation data has changed
	dirty = true;
}

// Add to the rotation
void Transform::Rotate(float pitch, float yaw, float roll)
{
	// Calculate the new rotation
	rotation = XMFLOAT3(rotation.x + pitch, rotation.y + yaw, rotation.z + roll);
	// A piece of transformation data has changed
	dirty = true;
}

