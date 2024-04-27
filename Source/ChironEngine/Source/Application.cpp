#include "Pch.h"
#include "Application.h"

#include "Modules/ModuleCamera.h"
#include "Modules/ModuleID3D12.h"
#include "Modules/ModuleInput.h"
#include "Modules/ModuleProgram.h"
#include "Modules/ModuleRender.h"
#include "Modules/ModuleWindow.h"

Application::Application(HWND hwnd, HINSTANCE hInstance) : _frameCounter(0), _elapsedSeconds(0.0)
{
	_modules.resize(static_cast<int>(ModuleType::LAST));
	_modules[static_cast<int>(ModuleToEnum<ModuleWindow>::value)] = std::make_unique<ModuleWindow>(hwnd, hInstance);
	_modules[static_cast<int>(ModuleToEnum<ModuleID3D12>::value)] = std::make_unique<ModuleID3D12>();
	_modules[static_cast<int>(ModuleToEnum<ModuleProgram>::value)] = std::make_unique<ModuleProgram>();
	_modules[static_cast<int>(ModuleToEnum<ModuleInput>::value)] = std::make_unique<ModuleInput>(hwnd);
	_modules[static_cast<int>(ModuleToEnum<ModuleCamera>::value)] = std::make_unique<ModuleCamera>(hwnd);
	_modules[static_cast<int>(ModuleToEnum<ModuleRender>::value)] = std::make_unique<ModuleRender>();
}

Application::~Application()
{
	_modules.clear();
}

bool Application::Init()
{
	for (const std::unique_ptr<Module>& module : _modules)
	{
		if (!module->Init())
		{
			return false;
		}
	}

	return true;
}

bool Application::Start()
{
	for (const std::unique_ptr<Module>& module : _modules)
	{
		if (!module->Start())
		{
			return false;
		}
	}

	return true;
}

UpdateStatus Application::Update()
{
	static std::chrono::high_resolution_clock clock;
	static auto t0 = clock.now();

	_frameCounter++;
	auto t1 = clock.now();
	auto deltaTime = t1 - t0;
	t0 = t1;

	_elapsedSeconds += deltaTime.count() * 1e-9;
	
	for (const std::unique_ptr<Module>& module : _modules)
	{
		UpdateStatus result = module->PreUpdate();
		if (result != UpdateStatus::UPDATE_CONTINUE)
		{
			return result;
		}
	}

	for (const std::unique_ptr<Module>& module : _modules)
	{
		UpdateStatus result = module->Update();
		if (result != UpdateStatus::UPDATE_CONTINUE)
		{
			return result;
		}
	}

	for (const std::unique_ptr<Module>& module : _modules)
	{
		UpdateStatus result = module->PostUpdate();
		if (result != UpdateStatus::UPDATE_CONTINUE)
		{
			return result;
		}
	}

	if (_elapsedSeconds > 1.0)
	{
		char buffer[500];
		auto fps = _frameCounter / _elapsedSeconds;
		sprintf_s(buffer, 500, "FPS: %f\n", fps);

		_frameCounter = 0;
		_elapsedSeconds = 0.0;
	}

	return UpdateStatus::UPDATE_CONTINUE;
}

bool Application::CleanUp()
{
	std::ranges::reverse_view reverseModules = std::ranges::reverse_view{ _modules };
	for (const std::unique_ptr<Module>& module : reverseModules)
	{
		if (!module->CleanUp())
		{
			return false;
		}
	}

	return true;
}
