#include "Camera.h"

// Constructors
// Every constructor needs an aspect ratio because that comes from the computer
Camera::Camera(float aspectRatio)
{
	// Call full constructor with defaults + aspect ratio
	Camera(XMFLOAT3(0, 0, 0), XMFLOAT3(0, 0, 0), aspectRatio, .5f, 1, 100, 1, 1);
}

Camera::Camera(XMFLOAT3 position, XMFLOAT3 orientation, float aspectRatio)
{
	// Call full constructor with defaults + 3 params
	Camera(position, orientation, aspectRatio, .5f, 1, 100, 1, 1);
}

Camera::Camera(XMFLOAT3 position, XMFLOAT3 orientation, float aspectRatio, float fov, float nearClip, float farClip, float moveSpeed, float rotSpeed)
{
	// Set up transform
	transform = Transform();
	transform.SetPosition(position.x, position.y, position.z);
	transform.SetRotation(orientation.x, orientation.y, orientation.z);

	// Set up the custom settings
	this->fov = fov;
	this->farClip = farClip;
	this->nearClip = nearClip;
	this->moveSpeed = moveSpeed;
	this->mouseSpeed = rotSpeed;

	// Set up the matrices
	UpdateProjectionMatrix(aspectRatio);
	UpdateViewMatrix();
}

// Basic Getters
XMFLOAT4X4 Camera::GetViewMatrix() { return viewMatrix; }
XMFLOAT4X4 Camera::GetProjMatrix() { return projMatrix; }

void Camera::UpdateProjectionMatrix(float aspectRatio)
{
	// Set new (temp) projection matrix
	XMMATRIX projMat = XMMatrixPerspectiveFovLH(fov, aspectRatio, nearClip, farClip);
	// Turn the matrix into a 4x4
	XMStoreFloat4x4(&projMatrix, projMat);
}

void Camera::UpdateViewMatrix()
{
	// Move into XMVector
	XMVECTOR position = XMVectorSet(transform.GetPosition().x, transform.GetPosition().y, transform.GetPosition().z, 0);
	// Get the forward vector rotated
	XMVECTOR vec = XMVectorSet(0, 0, 1, 0);
	XMVECTOR rotationVec = XMQuaternionRotationRollPitchYaw(transform.GetRotation().x, transform.GetRotation().y, transform.GetRotation().z);
	// Actual forward vector
	XMVECTOR direction = XMVector3Rotate(vec, rotationVec);
	// Calculate the view matrix
	XMMATRIX viewMat = XMMatrixLookToLH(position, direction, XMVectorSet(0, 1, 0, 0));
	// Actually store this
	XMStoreFloat4x4(&viewMatrix, viewMat);
}

// Update on delta time
void Camera::Update(float dt, HWND windowHandle)
{
	// Call the helper method
	CheckKeys(dt);

	// Mouse input
	// Define object for current mouse pos
	POINT mousePos = {};
	// Get current mouse pos
	GetCursorPos(&mousePos);
	// Coordinate system
	ScreenToClient(windowHandle, &mousePos);

	// Camera rotation on left mouse hold
	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
	{ 
		// Difference in mouse positions
		float xDiff = mousePos.x - prevMousePos.x;
		float yDiff = mousePos.y - prevMousePos.y;

		// Rotate the camera
		transform.Rotate(yDiff * dt * mouseSpeed, xDiff * dt * mouseSpeed, 0);
	}

	// Set previous mouse position
	prevMousePos = mousePos;

	// Update matrix based on the transform
	UpdateViewMatrix();
}


// Helper method for input
void Camera::CheckKeys(float dt)
{
	// Movement with keys
	if (GetAsyncKeyState('W') & 0x8000)
	{
		// Forward movement
		transform.LocalTranslate(0, 0, moveSpeed * dt);
	}
	if (GetAsyncKeyState('S') & 0x8000)
	{
		// Backward movement
		transform.LocalTranslate(0, 0, -moveSpeed * dt);
	}
	if (GetAsyncKeyState('D') & 0x8000)
	{
		// Right movement
		transform.LocalTranslate(moveSpeed * dt, 0, 0);
	}
	if (GetAsyncKeyState('A') & 0x8000)
	{
		// Left movement
		transform.LocalTranslate(-moveSpeed * dt, 0, 0);
	}
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		// World up movement
		transform.WorldTranslate(0, moveSpeed * dt, 0);
	}
	if (GetAsyncKeyState('X') & 0x8000)
	{
		// World down movement
		transform.WorldTranslate(0, -moveSpeed * dt, 0);
	}
}