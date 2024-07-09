#include "Pch.h"
#include "Application.h"

#include "Modules/ModuleCamera.h"
#include "Modules/ModuleID3D12.h"
#include "Modules/ModuleInput.h"
#include "Modules/ModuleProgram.h"
#include "Modules/ModuleRender.h"
#include "Modules/ModuleWindow.h"

#include "DataModels/Timer/Timer.h"

Application::Application(HWND hwnd, HINSTANCE hInstance) : _frameCount(0), _deltaTime(0)
{
	_modules.resize(static_cast<int>(ModuleType::LAST));
	_modules[static_cast<int>(ModuleToEnum<ModuleWindow>::value)] = std::make_unique<ModuleWindow>(hwnd, hInstance);
	_modules[static_cast<int>(ModuleToEnum<ModuleID3D12>::value)] = std::make_unique<ModuleID3D12>();
	_modules[static_cast<int>(ModuleToEnum<ModuleProgram>::value)] = std::make_unique<ModuleProgram>();
	_modules[static_cast<int>(ModuleToEnum<ModuleInput>::value)] = std::make_unique<ModuleInput>(hwnd);
	_modules[static_cast<int>(ModuleToEnum<ModuleCamera>::value)] = std::make_unique<ModuleCamera>();
	_modules[static_cast<int>(ModuleToEnum<ModuleRender>::value)] = std::make_unique<ModuleRender>();

	_timer = std::make_unique<Timer>();
}

Application::~Application()
{
	_modules.clear();
}

bool Application::Init()
{
	if (FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED)))
	{
		LOG_ERROR("Co Initialize Failed");
		return false;
	}

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
	_timer->Start();

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
	float beginFrame = _timer->Read();
	
	_frameCount++;

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

	float endFrame = _timer->Read();

	_deltaTime = (endFrame - beginFrame) / 1000.f;

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
