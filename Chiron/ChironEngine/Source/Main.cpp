#pragma once

#include "Pch.h"
#include "Application.h"

#include "Modules/ModuleWindow.h"

BOOL                CreateApplication(HINSTANCE hInstance);
ATOM                CreateWindowClass(HINSTANCE hInstance);
LRESULT CALLBACK    WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

std::unique_ptr<Application> App;

std::unique_ptr<ChironLog> logContext = std::make_unique<ChironLog>();

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	int mainReturn = EXIT_FAILURE;

    ChironLog::Init();

    LOG_INFO("Application Creation --------------");

    if (CreateApplication(hInstance) == FALSE)
    {
        return EXIT_FAILURE;
    }

    LOG_INFO("Application Init --------------");

    if (!App->Init())
    {
        LOG_ERROR("Application Init exits with error -----");
        return EXIT_FAILURE;
    }

    LOG_INFO("Application Start --------------");
    
    if (!App->Start())
    {
        LOG_ERROR("Application Start exits with error -----");
        return EXIT_FAILURE;
    }

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnregisterClass(WND_CLASS_NAME, hInstance);

	LOG_INFO("Bye :)\n");

	return EXIT_SUCCESS;
}

BOOL CreateApplication(HINSTANCE hInstance)
{
    if (CreateWindowClass(hInstance) == 0)
    {
        assert(false && "Error creating Window Class.");
    }
    
    HWND hwnd = CreateWindowW(
        WND_CLASS_NAME,         // Window Class
        WND_NAME,               // Window title
        WS_OVERLAPPEDWINDOW,    // Window style
        CW_USEDEFAULT,          // Position x
        CW_USEDEFAULT,          // Position y
        CW_USEDEFAULT,          // Widht
        CW_USEDEFAULT,          // Height
        nullptr,                // Principal window (NULL bcs this is the principal one)
        nullptr,                // Menu (NULL bcs I don't use a menu)
        hInstance,              // Hinstance
        nullptr                 // Additional parameter (NULL en este caso)
    );

    if (hwnd == NULL)
    {
        return FALSE;
    }
    App = std::make_unique<Application>(hwnd, hInstance);

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);


    return TRUE;
}

ATOM CreateWindowClass(HINSTANCE hIstance)
{
    WNDCLASSEXW wcex {};

    // Fill in the window class structure with parameters 
    // that describe the main window. 

    wcex.cbSize = sizeof(WNDCLASSEXW);                  // size of structure 
    wcex.lpszClassName = WND_CLASS_NAME;                // name of window class 
    wcex.style = CS_HREDRAW | CS_VREDRAW;               // redraw if size changes 
    wcex.lpfnWndProc = WndProc;                         // points to window procedure 
    wcex.cbClsExtra = 0;                                // no extra class memory 
    wcex.cbWndExtra = 0;                                // no extra window memory 
    wcex.hInstance = hIstance;                          // handle to instance 
    wcex.hIcon = LoadIcon(nullptr, IDI_APPLICATION);       // predefined app. icon 
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);         // predefined arrow 
    wcex.lpszMenuName = NULL;                           // name of menu resource 
    wcex.hIconSm = LoadIcon(nullptr, IDI_APPLICATION); // small class icon

    // Register the window class. 
    return RegisterClassExW(&wcex);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_ACTIVATE:
    case WM_ACTIVATEAPP:
        DirectX::Keyboard::ProcessMessage(message, wParam, lParam);
        DirectX::Mouse::ProcessMessage(message, wParam, lParam);
        break;
    case WM_INPUT:
    case WM_MOUSEMOVE:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
    case WM_MOUSEWHEEL:
    case WM_XBUTTONDOWN:
    case WM_XBUTTONUP:
    case WM_MOUSEHOVER:
        DirectX::Mouse::ProcessMessage(message, wParam, lParam);
        break;
    case WM_COMMAND:
    case WM_PAINT:
        App->Update();
        break;
    case WM_DESTROY:
        LOG_INFO("Application CleanUp --------------");
        if (!App->CleanUp())
        {
            LOG_ERROR("Application CleanUp exits with error -----");
        }
        PostQuitMessage(0);
        break;
    case WM_SIZE:
        App->GetModule<ModuleWindow>()->Resize();
        break;
    case WM_SYSKEYDOWN:
        DirectX::Keyboard::ProcessMessage(message, wParam, lParam);
        break;
    case WM_KEYDOWN:
        CHIRON_TODO("Review functionality.");
        if (wParam == VK_F11)
        {
            App->GetModule<ModuleWindow>()->ToggleFullScreen();
        }
        DirectX::Keyboard::ProcessMessage(message, wParam, lParam);
        break;
    case WM_KEYUP:
    case WM_SYSKEYUP:
        DirectX::Keyboard::ProcessMessage(message, wParam, lParam);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

