#include "Pch.h"
#include "ModuleCamera.h"

#include "Application.h"

#include "ModuleWindow.h"

ModuleCamera::ModuleCamera() : _fov(45.f), _nearPlane(0.1f), _farPlane(10000.f)
{
}

ModuleCamera::~ModuleCamera()
{
}

bool ModuleCamera::Init()
{
	unsigned width;
	unsigned height;

	App->GetModule<ModuleWindow>()->GetWindowSize(width, height);
	float aspectRatio = static_cast<float>(width / height);

	_position = Vector3(0, 10, 10);

	_rotation = Quaternion::Identity;

	_view = Matrix::CreateLookAt(_position, Vector3::Zero, Vector3::UnitY);
	_proj = Matrix::CreatePerspectiveFieldOfView(DirectX::XM_PIDIV4, aspectRatio, _nearPlane, _farPlane);
	return true;
}

UpdateStatus ModuleCamera::PreUpdate()
{
	return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus ModuleCamera::Update()
{
	DirectX::Mouse& mouse = DirectX::Mouse::Get();
	DirectX::Keyboard& keyboard = DirectX::Keyboard::Get();

	const DirectX::Mouse::State& mouseState = mouse.GetState();
	const DirectX::Keyboard::State& keyState = keyboard.GetState();

	CHIRON_TODO("Finish.");
	/*if (mouseState.rightButton)
	{

	}*/

	

	return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus ModuleCamera::PostUpdate()
{
	return UpdateStatus::UPDATE_CONTINUE;
}

bool ModuleCamera::CleanUp()
{
	return true;
}
