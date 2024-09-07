#include "Pch.h"
#include "ModuleInput.h"

ModuleInput::ModuleInput(HWND hwnd) : _mousePos(Vector2::Zero), _captureMousePos(false)
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
	const auto& mouseState = _mouse->GetState();
	_mousePos = Vector2(static_cast<float>(mouseState.x), static_cast<float>(mouseState.y));

	return true;
}

UpdateStatus ModuleInput::PreUpdate()
{
	const auto& mouseState = _mouse->GetState();
	if (mouseState.positionMode == DirectX::Mouse::MODE_ABSOLUTE)
	{
		if (_captureMousePos)
		{
			_mousePos = Vector2(static_cast<float>(mouseState.x), static_cast<float>(mouseState.y));
			_captureMousePos = false;
		}
	}
	return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus ModuleInput::Update()
{
	return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus ModuleInput::PostUpdate()
{
	const auto& mouseState = _mouse->GetState();
	_mousePos = Vector2(static_cast<float>(mouseState.x), static_cast<float>(mouseState.y));

	return UpdateStatus::UPDATE_CONTINUE;
}

bool ModuleInput::CleanUp()
{
	return true;
}
