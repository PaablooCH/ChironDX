#pragma once

#include "Modules/Module.h"
#include "Enums/ModuleType.h"

class Timer;

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

    // ------------- GETTERS ----------------------

    template<typename M>
    M* GetModule();

    inline const uint64_t GetFrameCount() const;
    inline const float GetFPS() const;
    inline const float GetDeltaTime() const;

private:
    std::vector<std::unique_ptr<Module>> _modules;

    std::unique_ptr<Timer> _timer;

    uint64_t _frameCount;

    float _deltaTime;
};

template<typename M>
inline M* Application::GetModule()
{
    int index = static_cast<int>(ModuleToEnum<M>::value);
    return static_cast<M*>(_modules[index].get());
}

inline const uint64_t Application::GetFrameCount() const
{
    return _frameCount;
}

inline const float Application::GetFPS() const
{
    return 1 / _deltaTime;
}

inline const float Application::GetDeltaTime() const
{
    return _deltaTime;
}

extern std::unique_ptr<Application> App;
