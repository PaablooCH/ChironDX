#pragma once
#include "Module.h"

class ModuleInput : public Module
{
public:
    ModuleInput() = delete;
    ModuleInput(HWND hwnd);
    ~ModuleInput() override;

    bool            Init() override;
    UpdateStatus    PreUpdate() override;
    UpdateStatus    Update() override;
    bool            CleanUp() override;

    inline DirectX::Keyboard*   GetKeyboard();
    inline DirectX::Mouse*      GetMouse();
private:
    std::unique_ptr<DirectX::Keyboard> _keyboard;
    std::unique_ptr<DirectX::Mouse> _mouse;
};

inline DirectX::Keyboard* ModuleInput::GetKeyboard()
{
    return _keyboard.get();
}

inline DirectX::Mouse* ModuleInput::GetMouse()
{
    return _mouse.get();
}
