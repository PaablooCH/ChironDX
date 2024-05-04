#include "Pch.h"
#include "ModuleInput.h"

ModuleInput::ModuleInput(HWND hwnd)
{
	_keyboard = std::make_unique<DirectX::Keyboard>();
	_mouse = std::make_unique<DirectX::Mouse>();
	_mouse->SetWindow(hwnd);
}

ModuleInput::~ModuleInput()
{
}

bool ModuleInput::Init()
{
	return true;
}

UpdateStatus ModuleInput::PreUpdate()
{
	return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus ModuleInput::Update()
{
	return UpdateStatus::UPDATE_CONTINUE;
}

bool ModuleInput::CleanUp()
{
	return true;
}
