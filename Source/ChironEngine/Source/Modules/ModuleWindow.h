#pragma once
#include "Module.h"

class ModuleWindow : public Module
{
public:
    ModuleWindow() = delete;
    ModuleWindow(HWND hwnd, HINSTANCE hInstance);
    ~ModuleWindow() override;

    bool Init() override;
    UpdateStatus PreUpdate() override;
    UpdateStatus Update() override;
    UpdateStatus PostUpdate() override;
    bool CleanUp() override;

    // ------------- WINDOW FUNC ----------------------

    void Resize();
    void Resize(unsigned width, unsigned height);
    void ToggleFullScreen();

    // ------------- WINDOW CURSOR ----------------------

    void UnlimitedCursor() const;

    // ------------- GETTERS ----------------------

    inline HWND GetWindowId() const;
    inline void GetWindowSize(unsigned& width, unsigned& height) const;

private:
    HWND _hWnd; // window ID
    HINSTANCE _hInstance;

    // By default, use windowed mode.
    // Can be toggled with F11
    bool _fullscreen;

    unsigned _width;
    unsigned _height;
    RECT _lastWindowRect;
};

inline HWND ModuleWindow::GetWindowId() const
{
    return _hWnd;
}

inline void ModuleWindow::GetWindowSize(unsigned& width, unsigned& height) const
{
    width = _width;
    height = _height;
}