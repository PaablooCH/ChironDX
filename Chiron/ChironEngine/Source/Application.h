#pragma once

#include "Modules/Module.h"
#include "Enums/ModuleType.h"

class Application
{
public:
	Application() = delete;
	Application(HWND hwnd, HINSTANCE hInstance);
	~Application();

	bool Init();
	bool Start();
	UpdateStatus Update();
	bool CleanUp();

	template<typename M>
	M* GetModule();

	inline const double GetFPS() const;

private:
	std::vector<std::unique_ptr<Module>> _modules;
	
	uint64_t _frameCounter;
	double _elapsedSeconds;
};

template<typename M>
inline M* Application::GetModule()
{
	int index = static_cast<int>(ModuleToEnum<M>::value);
	return static_cast<M*>(_modules[index].get());
}

inline const double Application::GetFPS() const
{
	return _frameCounter / _elapsedSeconds;
}

extern std::unique_ptr<Application> App;
