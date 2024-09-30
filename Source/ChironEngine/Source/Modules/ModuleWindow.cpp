#include "Pch.h"
#include "ModuleWindow.h"

#include "Application.h"

#include "ModuleID3D12.h"
#include "ModuleInput.h"
#include "ModuleRender.h"

ModuleWindow::ModuleWindow(HWND hwnd, HINSTANCE hInstance) : _hWnd(hwnd), _hInstance(hInstance), _fullscreen(false),
_width(0), _height(0), _lastWindowRect()
{
}

ModuleWindow::~ModuleWindow()
{
}

bool ModuleWindow::Init()
{
    if (_hWnd == NULL)
    {
        return false;
    }

    RECT rect;
    if (GetWindowRect(_hWnd, &rect))
    {
        _width = static_cast<unsigned>(rect.right - rect.left);
        _height = static_cast<unsigned>(rect.bottom - rect.top);
    }
    else
    {
        return false;
    }

    // Drag and drop
    DragAcceptFiles(_hWnd, TRUE);

    return true;
}

bool ModuleWindow::Start()
{
    ToggleFullScreen();
    return true;
}

UpdateStatus ModuleWindow::PreUpdate()
{
    return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus ModuleWindow::Update()
{
    return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus ModuleWindow::PostUpdate()
{
    return UpdateStatus::UPDATE_CONTINUE;
}

bool ModuleWindow::CleanUp()
{
    return true;
}

void ModuleWindow::Resize()
{
    unsigned width = 0;
    unsigned height = 0;
    RECT rect;
    if (GetWindowRect(_hWnd, &rect))
    {
        width = static_cast<unsigned>(rect.right - rect.left);
        height = static_cast<unsigned>(rect.bottom - rect.top);
    }
    else
    {
        LOG_WARNING("Can not obtain the new width and height values.");
    }

    Resize(width, height);
}

void ModuleWindow::Resize(unsigned width, unsigned height)
{
    if (_width != width || _height != height)
    {
        // Don't allow 0 size swap chain back buffers.
        _width = std::max(1u, width);
        _height = std::max(1u, height);

        // Flush the GPU queue to make sure the swap chain's back buffers are not being referenced by an in-flight
        // command list.
        App->GetModule<ModuleID3D12>()->ResizeBuffers();
        App->GetModule<ModuleRender>()->ResizeBuffers(_width, _height);
    }
}

void ModuleWindow::ToggleFullScreen()
{
    _fullscreen = !_fullscreen;

    if (_fullscreen)
    {
        // Store the current window dimensions so they can be restored when switching out of fullscreen state.
        ::GetWindowRect(_hWnd, &_lastWindowRect);

        // Set the window style to a borderless window so the client area fills the entire screen.
        UINT windowStyle = WS_OVERLAPPEDWINDOW & ~(WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);

        ::SetWindowLongW(_hWnd, GWL_STYLE, windowStyle);

        // Query the name of the nearest display device for the window.
        // This is required to set the fullscreen dimensions of the window when using a multi-monitor setup.
        HMONITOR hMonitor = ::MonitorFromWindow(_hWnd, MONITOR_DEFAULTTONEAREST);
        MONITORINFOEX monitorInfo = {};
        monitorInfo.cbSize = sizeof(MONITORINFOEX);
        ::GetMonitorInfo(hMonitor, &monitorInfo);

        ::SetWindowPos(_hWnd, HWND_TOP,
            monitorInfo.rcMonitor.left,
            monitorInfo.rcMonitor.top,
            monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
            monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
            SWP_FRAMECHANGED | SWP_NOACTIVATE);

        ::ShowWindow(_hWnd, SW_MAXIMIZE);
    }
    else
    {
        // Restore all the window decorators.
        ::SetWindowLong(_hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);

        ::SetWindowPos(_hWnd, HWND_NOTOPMOST,
            _lastWindowRect.left,
            _lastWindowRect.top,
            _lastWindowRect.right - _lastWindowRect.left,
            _lastWindowRect.bottom - _lastWindowRect.top,
            SWP_FRAMECHANGED | SWP_NOACTIVATE);

        ::ShowWindow(_hWnd, SW_NORMAL);
    }
}

void ModuleWindow::UnlimitedCursor() const
{
    auto input = App->GetModule<ModuleInput>();
    POINT cursorPos;
    GetCursorPos(&cursorPos);

    // Get the dimensions of the current window
    RECT windowRect;
    GetClientRect(_hWnd, &windowRect);
    MapWindowPoints(_hWnd, nullptr, reinterpret_cast<POINT*>(&windowRect), 2); // Convert to screen coordinates

    if (cursorPos.x < windowRect.left)
    {
        SetCursorPos(windowRect.right - 1, cursorPos.y); // Move to the right side
        input->CaptureMousePos();
    }
    else if (cursorPos.x >= windowRect.right)
    {
        SetCursorPos(windowRect.left + 1, cursorPos.y); // Move to the left side
        input->CaptureMousePos();
    }

    if (cursorPos.y < windowRect.top)
    {
        SetCursorPos(cursorPos.x, windowRect.bottom - 1); // Move to the bottom edge
        input->CaptureMousePos();
    }
    else if (cursorPos.y >= windowRect.bottom)
    {
        SetCursorPos(cursorPos.x, windowRect.top + 1); // Move to the top edge
        input->CaptureMousePos();
    }
}