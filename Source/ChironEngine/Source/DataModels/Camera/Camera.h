#pragma once

#define DEFAULT_MOVE_SPEED 9.f
#define DEFAULT_ROTATION_DEGREE 30
#define DEFAULT_ROTATION_SPEED 5.f
#define DEFAULT_MOUSE_SPEED_MODIFIER 0.f
#define DEFAULT_MOUSE_ZOOM_SPEED 2.f
#define DEFAULT_SHIFT_ACCELERATION 2.f
#define MOUSE_SPEED_PERCENTATGE 0.05f

class Camera
{
public:
    Camera(Vector3 position, Vector3 front, Vector3 up, float fov, float nearPlane, float farPlane);
    ~Camera();

    void Update();

    // ------------- GETTERS ----------------------

    inline const Vector3& GetPosition() const;
    inline const Quaternion& GetRotation() const;
    inline const Matrix& GetViewMatrix() const;
    inline const Matrix& GetProjMatrix() const;
    inline float GetFOV() const;
    inline float GetAspectRatio() const;
    inline float GetNearPlane() const;
    inline float GetFarPlane() const;

    // ------------- SETTERS ----------------------

    inline void SetFOV(float fov);
    inline void SetAspectRatio(float aspectRatio);
    inline void SetNearPlane(float nearPlane);
    inline void SetFarPlane(float farPlane);

private:
    void Move();
    void FreeLook();

    void ApplyRotationWithFixedUp(const Quaternion& rotationQuat, const Vector3& fixedUp);

    inline float RunSpeed();
    inline float WalkSpeed();

    inline void RecalculateProjectMatrix();

private:
    Vector3 _position;
    Vector3 _forward;
    Vector3 _up;
    Quaternion _rotation;

    float _moveSpeed;
    float _acceleration;
    float _rotationSpeed;

    float _fov;
    float _aspectRatio;
    float _nearPlane;
    float _farPlane;

    Matrix _view;
    Matrix _proj;
};

inline const Vector3& Camera::GetPosition() const
{
    return _position;
}

inline const Quaternion& Camera::GetRotation() const
{
    return _rotation;
}

inline const Matrix& Camera::GetViewMatrix() const
{
    return _view;
}

inline const Matrix& Camera::GetProjMatrix() const
{
    return _proj;
}

inline float Camera::GetFOV() const
{
    return _fov;
}

inline float Camera::GetAspectRatio() const
{
    return _aspectRatio;
}

inline float Camera::GetNearPlane() const
{
    return _nearPlane;
}

inline float Camera::GetFarPlane() const
{
    return _farPlane;
}

inline void Camera::SetFOV(float fov)
{
    _fov = fov;
    RecalculateProjectMatrix();
}

inline void Camera::SetAspectRatio(float aspectRatio)
{
    _aspectRatio = aspectRatio;
    RecalculateProjectMatrix();
}

inline void Camera::SetNearPlane(float nearPlane)
{
    _nearPlane = nearPlane;
    RecalculateProjectMatrix();
}

inline void Camera::SetFarPlane(float farPlane)
{
    _farPlane = farPlane;
    RecalculateProjectMatrix();
}

inline float Camera::RunSpeed()
{
    _acceleration = DEFAULT_SHIFT_ACCELERATION;
    return _moveSpeed * _acceleration;
}

inline float Camera::WalkSpeed()
{
    _acceleration = 1.f;
    return _moveSpeed * _acceleration;
}

inline void Camera::RecalculateProjectMatrix()
{
    _proj = Matrix::CreatePerspectiveFieldOfView(_fov, _aspectRatio, _nearPlane, _farPlane);
}
