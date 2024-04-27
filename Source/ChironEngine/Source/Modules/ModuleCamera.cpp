#include "Pch.h"
#include "ModuleCamera.h"

ModuleCamera::ModuleCamera()
{
}

ModuleCamera::~ModuleCamera()
{
}

bool ModuleCamera::Init()
{
	_position = Vector3::Zero;
	return true;
}

UpdateStatus ModuleCamera::PreUpdate()
{
	return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus ModuleCamera::Update()
{
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
