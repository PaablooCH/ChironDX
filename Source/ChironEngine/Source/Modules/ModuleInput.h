#pragma once
#include "Module.h"

class ModuleInput : public Module
{
public:
    ModuleInput() = delete;
    ModuleInput(HWND hwnd);
    ~ModuleInput() override;

    bool Init() override;
    UpdateStatus PreUpdate() override;
    UpdateStatus Update() override;
    UpdateStatus PostUpdate() override;
    bool CleanUp() override;

    inline const Vector2 MouseRel();
    inline void CaptureMousePos();

private:
    std::unique_ptr<DirectX::Keyboard> _keyboard;
    std::unique_ptr<DirectX::Mouse> _mouse;

    Vector2 _mousePos;

    bool _captureMousePos;
};

inline const Vector2 ModuleInput::MouseRel()
{
    const auto& mouseState = _mouse->GetState();
    if (mouseState.positionMode == DirectX::Mouse::MODE_ABSOLUTE)
    {
        return Vector2(static_cast<float>(mouseState.x - _mousePos.x), static_cast<float>(mouseState.y - _mousePos.y));
    }
    else // MODE_RELATIVE
    {
        _captureMousePos = true;
        return Vector2(static_cast<float>(mouseState.x), static_cast<float>(mouseState.y));
    }
}

inline void ModuleInput::CaptureMousePos()
{
    _captureMousePos = true;
}
