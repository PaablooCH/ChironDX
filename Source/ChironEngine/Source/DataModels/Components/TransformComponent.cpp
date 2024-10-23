#include "Pch.h"
#include "TransformComponent.h"

TransformComponent::TransformComponent(GameObject* owner) : Component(ComponentType::TRANSFORM, owner),
    _localPos(Vector3::Zero),
    _localRot(Quaternion::Identity),
    _localSca(Vector3::One),
    _localMatrix(Matrix::Identity),
    _globalPos(Vector3::Zero),
    _globalRot(Quaternion::Identity),
    _globalSca(Vector3::One),
    _globalMatrix(Matrix::Identity),
    _bbPos(Vector3::Zero),
    _bbSca(Vector3::One),
    _originScaling(Vector3(0.5f, 0.5f, 0.5f)),
    _originCenter(Vector3(0.5f, 0.5f, 0.5f)),
    _rotXYZ(Vector3::Zero),
    _localAABB({ { 0, 0, 0 }, { 1, 1, 1 } }),
    _encapsuledAABB(_localAABB),
    _objectOBB({ { 0, 0, 0 }, { 1, 1, 1 }, Vector4::UnitW }),
    _drawBoundingBoxes(false),
    _uniformScale(true)
{
}

TransformComponent::TransformComponent(const TransformComponent& copy) : Component(copy), 
    _localPos(copy._localPos),
    _localRot(copy._localRot),
    _localSca(copy._localSca),
    _localMatrix(copy._localMatrix),
    _globalPos(copy._globalPos),
    _globalRot(copy._globalRot),
    _globalSca(copy._globalSca),
    _globalMatrix(copy._globalMatrix),
    _bbPos(copy._bbPos),
    _bbSca(copy._bbSca),
    _originScaling(copy._originScaling),
    _originCenter(copy._originCenter),
    _rotXYZ(copy._rotXYZ),
    _localAABB(copy._localAABB),
    _encapsuledAABB(copy._encapsuledAABB),
    _objectOBB(copy._objectOBB),
    _drawBoundingBoxes(copy._drawBoundingBoxes),
    _uniformScale(copy._uniformScale)
{
}

TransformComponent::~TransformComponent()
{
}
