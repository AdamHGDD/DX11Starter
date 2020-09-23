#pragma once
#include <Windows.h>
#include "Transform.h"

class Camera
{
private:
	// Fields
	// View matrix that is defined by the transform
	XMFLOAT4X4 viewMatrix;
	// Secondary information about the camera like view distance, aspect ration and orthographic
	XMFLOAT4X4 projMatrix;
	// Previous mouse position
	POINT prevMousePos;
	// Information for customization
	// Field of view
	float fov;
	// Distance to near clipping plane
	float nearClip;
	// Distance to far clipping plane
	float farClip;
	// Physical move speed
	float moveSpeed;
	// Rotation speed
	float mouseSpeed;

	// Helper Methods
	void CheckKeys(float dt);
public:
	// Transform data
	Transform transform;

	// Constructors
	// Every constructor needs an aspect ratio because that comes from the computer
	Camera(float aspectRatio);
	// Default for customization but specified position and orientation
	Camera(XMFLOAT3 position, XMFLOAT3 orientation, float aspectRatio);
	// All variables set
	Camera(XMFLOAT3 position, XMFLOAT3 orientation, float aspectRatio, float fov, float nearClip, float farClip, float moveSpeed, float rotSpeed);
	
	// Methods - Getters
	XMFLOAT4X4 GetViewMatrix();
	XMFLOAT4X4 GetProjMatrix();
	
	// Methods - Update Matrices
	void UpdateProjectionMatrix(float aspectRatio);
	void UpdateViewMatrix();

	// Method - Loop
	void Update(float dh, HWND windowHandle);
};

