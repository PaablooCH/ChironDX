#include "Pch.h"
#include "Camera.h"

#include "Application.h"

#include "Modules/ModuleInput.h"
#include "Modules/ModuleWindow.h"

Camera::Camera(Vector3 position, Vector3 front, Vector3 up, float fov, float nearPlane, float farPlane) : 
	_position(position), _forward(front), _up(up), _rotation(Quaternion::Identity), _moveSpeed(DEFAULT_MOVE_SPEED), 
	_acceleration(1.f), _rotationSpeed(DEFAULT_ROTATION_SPEED), _fov(fov), _nearPlane(nearPlane), _farPlane(farPlane)
{
	unsigned width;
	unsigned height;

	App->GetModule<ModuleWindow>()->GetWindowSize(width, height);
	float aspectRatio = static_cast<float>(width) / static_cast<float>(height);

	_forward.Normalize();
	_up.Normalize();
	_view = Matrix::CreateLookAt(_position, _forward, _up);
	_proj = Matrix::CreatePerspectiveFieldOfView(_fov, aspectRatio, _nearPlane, _farPlane);
}

Camera::~Camera()
{
}

void Camera::Update()
{
	DirectX::Mouse& mouse = DirectX::Mouse::Get();
	DirectX::Keyboard& keyboard = DirectX::Keyboard::Get();
	const DirectX::Mouse::State& mouseState = mouse.GetState();
	const DirectX::Keyboard::State& keyState = keyboard.GetState();

	if (mouseState.rightButton)
	{

		float deltaTime = App->GetDeltaTime();
		float moveSpeed = (keyState.LeftShift ? RunSpeed() : WalkSpeed()) * deltaTime;

		_forward.Normalize();
		_up.Normalize();
		Vector3 right = _forward.Cross(_up).Normalized();

		// Movement
		if (keyState.W)
		{
			// Forward
			_position += _forward * moveSpeed;
		}
		if (keyState.S)
		{
			// Backward
			_position -= _forward * moveSpeed;
		}
		if (keyState.A)
		{
			// Left
			_position -= right * moveSpeed;
		}
		if (keyState.D)
		{
			// Right
			_position += right * moveSpeed;
		}
		if (keyState.Q)
		{
			// Up
			_position += _up * moveSpeed;
		}
		if (keyState.E)
		{
			// Down
			_position -= _up * moveSpeed;
		}
		FreeLook();
		
	}
	_view = Matrix::CreateLookAt(_position, _position + _forward, _up);
}

void Camera::FreeLook()
{
	auto input = App->GetModule<ModuleInput>();
	float deltaTime = App->GetDeltaTime();

	const Vector2 rel = input->MouseRel();
	float rotationSpeed = _rotationSpeed * MOUSE_SPEED_PERCENTATGE * deltaTime;
	float xrel = -rel.x * rotationSpeed;
	float yrel = -rel.y * rotationSpeed;

	Vector3 right = _forward.Cross(_up).Normalized();
	Quaternion rotationX = Quaternion::CreateFromAxisAngle(Vector3::UnitY, xrel);
	Quaternion rotationY = Quaternion::CreateFromAxisAngle(right, yrel);

	Quaternion combinedRotation = rotationY * rotationX;

	ApplyRotationWithFixedUp(combinedRotation, Vector3::UnitY);
}

void Camera::ApplyRotationWithFixedUp(const Quaternion& rotationQuat, const Vector3& fixedUp)
{
	_forward = Vector3::Transform(_forward, rotationQuat).Normalized();
	Vector3 newRight = Vector3::UnitY.Cross(_forward).Normalized();
	_up = _forward.Cross(newRight).Normalized();

	_rotation = rotationQuat;
}