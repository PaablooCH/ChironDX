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

    // ------------- GETTERS ----------------------

    inline const Vector3& GetPosition();
    inline const Quaternion& GetRotation();
    inline const Matrix& GetViewMatrix();
    inline const Matrix& GetProjMatrix();

private:
    float _fov;

    Vector3 _position;
    Quaternion _rotation;

    Matrix _view;
    Matrix _proj;

    float _nearPlane;
    float _farPlane;
};

inline const Vector3& ModuleCamera::GetPosition()
{
    return _position;
}

inline const Quaternion& ModuleCamera::GetRotation()
{
    return _rotation;
}

inline const Matrix& ModuleCamera::GetViewMatrix()
{
    return _view;
}

inline const Matrix& ModuleCamera::GetProjMatrix()
{
    return _proj;
}
