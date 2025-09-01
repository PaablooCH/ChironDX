#include "Pch.h"
#include "ModuleInput.h"

#include "Application.h"

#include "ModuleWindow.h"

#ifdef PROFILE
    #include "Optick/optick.h"
#endif // OPTICK

ModuleInput::ModuleInput() : _mousePos(Vector2::Zero), _captureMousePos(false)
{
}

ModuleInput::~ModuleInput()
{
}

UpdateStatus ModuleInput::PreUpdate()
{
#ifdef PROFILE
    OPTICK_CATEGORY("PreUpdateInput", Optick::Category::Input);
#endif // DEBUG
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
#ifdef PROFILE
    OPTICK_CATEGORY("PostUpdateInput", Optick::Category::Input);
#endif // DEBUG
    const auto& mouseState = _mouse->GetState();
    _mousePos = Vector2(static_cast<float>(mouseState.x), static_cast<float>(mouseState.y));

    return UpdateStatus::UPDATE_CONTINUE;
}

bool ModuleInput::CleanUp()
{
    return true;
}

void ModuleInput::StartCapturing()
{
    _keyboard = std::make_unique<DirectX::Keyboard>();
    _mouse = std::make_unique<DirectX::Mouse>();
    _mouse->SetWindow(App->GetModule<ModuleWindow>()->GetWindowId());

    const auto& mouseState = _mouse->GetState();
    _mousePos = Vector2(static_cast<float>(mouseState.x), static_cast<float>(mouseState.y));
}
