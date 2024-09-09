#pragma once
#include "Module.h"

class Camera;

class ModuleCamera : public Module
{
public:
    ModuleCamera();
    ~ModuleCamera() override;

    bool Init() override;
    UpdateStatus PreUpdate() override;
    UpdateStatus Update() override;
    UpdateStatus PostUpdate() override;
    bool CleanUp() override;

    // ------------- GETTERS ----------------------

    inline Camera* GetCamera();

private:
    std::unique_ptr<Camera> _camera;
};

inline Camera* ModuleCamera::GetCamera()
{
    return _camera.get();
}