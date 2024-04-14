#pragma once
#include "Module.h"

class ModuleWindow : public Module
{
public:
    ModuleWindow() = delete;
    ModuleWindow(HWND hwnd, HINSTANCE hInstance);
    ~ModuleWindow() override;

    bool            Init() override;
    UpdateStatus    PreUpdate() override;
    UpdateStatus    Update() override;
    UpdateStatus    PostUpdate() override;
    bool            CleanUp() override;

    void            Resize();
    void            ToggleFullScreen();

private:
    HWND        _hwnd; // window ID
    HINSTANCE   _hInstance;
};

