#pragma once
#include "Module.h"

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

private:
    Vector3 _position;
    Quaternion _rotation;

    Matrix _view;
    Matrix _proj;
};

