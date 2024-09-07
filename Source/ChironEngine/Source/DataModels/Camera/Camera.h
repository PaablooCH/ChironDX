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

    inline const Vector3& GetPosition();
    inline const Quaternion& GetRotation();
    inline const Matrix& GetViewMatrix();
    inline const Matrix& GetProjMatrix();

protected:
	inline float RunSpeed();
	inline float WalkSpeed();

private:
    void Move();
    void FreeLook();
    void ApplyRotationWithFixedUp(const Quaternion& rotationQuat, const Vector3& fixedUp);

private:
    Vector3 _position;
    Vector3 _forward;
    Vector3 _up;
    Quaternion _rotation;

    float _moveSpeed;
    float _acceleration;
    float _rotationSpeed;

    float _fov;
    float _nearPlane;
    float _farPlane;

    Matrix _view;
    Matrix _proj;
};

inline const Vector3& Camera::GetPosition()
{
    return _position;
}

inline const Quaternion& Camera::GetRotation()
{
    return _rotation;
}

inline const Matrix& Camera::GetViewMatrix()
{
    return _view;
}

inline const Matrix& Camera::GetProjMatrix()
{
    return _proj;
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
