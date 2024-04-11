#pragma once

#include "Modules/Module.h"
#include "Enums/ModuleType.h"

class Application
{
public:
	Application();
	~Application();

	bool Init();
	bool Start();
	UpdateStatus Update();
	bool CleanUp();

	template<typename M>
	M* GetModule();

private:
	std::vector<std::unique_ptr<Module>> _modules;
	float _deltaTime = 0.f;
};

template<typename M>
inline M* Application::GetModule()
{
	int index = static_cast<int>(ModuleToEnum<M>::value);
	return static_cast<M*>(_modules[index].get());
}

extern std::unique_ptr<Application> App;
