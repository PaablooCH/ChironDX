#include "Pch.h"
#include "ModuleWindow.h"
#include "Application.h"

ModuleWindow::ModuleWindow(HWND hwnd, HINSTANCE hInstance) : _hwnd(hwnd), _hInstance(hInstance)
{
}

ModuleWindow::~ModuleWindow()
{
}

bool ModuleWindow::Init()
{
    if (_hwnd == NULL)
    {
        return false;
    }

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
}

void ModuleWindow::ToggleFullScreen()
{
}