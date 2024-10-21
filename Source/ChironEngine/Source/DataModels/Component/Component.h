#pragma once

#include "Enums/ComponentType.h"

class GameObject;

class Component
{
public:
    Component(ComponentType type, GameObject* owner);
    Component(const Component& copy);
    virtual ~Component();

    // ------------- GETTERS ----------------------

    inline ComponentType GetType() const;
    inline GameObject* GetOwner();
    inline bool& IsEnabled();
    bool IsActive();

private:
    ComponentType _type;

    GameObject* _owner;
    bool _enabled;
};

inline ComponentType Component::GetType() const
{
    return _type;
}

inline GameObject* Component::GetOwner()
{
    return _owner;
}

inline bool& Component::IsEnabled()
{
    return _enabled;
}
