#include "Pch.h"
#include "GameObject.h"

#include "Application.h"

#include "Modules/ModuleScene.h"

#include "DataModels/Components/TransformComponent.h"
#include "DataModels/Components/Interfaces/Drawable.h"
#include "DataModels/Components/Interfaces/Updatable.h"

#include "DataModels/Scene/Scene.h"

// root constructor
GameObject::GameObject(const std::string& name) : GameObject(name, nullptr, true, true, false)
{
}

GameObject::GameObject(const std::string& name, GameObject* parent) : GameObject(name, parent, true, 
    parent->IsActive(), parent->IsStatic())
{
}

GameObject::GameObject(const std::string& name, GameObject* parent, bool enabled, bool active, bool staticObject)
    : _name(name), _parent(parent), _enabled(enabled), _active(active), _static(staticObject)
{
    if (_static)
    {
        App->GetModule<ModuleScene>()->GetLoadedScene()->AddStaticGO(this);
    }
    else
    {
        App->GetModule<ModuleScene>()->GetLoadedScene()->AddDynamicGO(this);
    }
}

GameObject::GameObject(const GameObject& copy) : GameObject(copy._name, copy._parent, copy._enabled, copy._active, copy._static)
{
    std::ranges::for_each(copy._components.begin(), copy._components.end(),
        [this](const std::unique_ptr<Component>& copyComponent)
        {
            this->CopyComponent(copyComponent.get());
        });

    std::ranges::for_each(copy._children.begin(), copy._children.end(),
        [this](const std::unique_ptr<GameObject>& copyChild)
        {
            GameObject* newChild = new GameObject(*copyChild.get());
            this->LinkChild(newChild);
        });
}

GameObject::~GameObject()
{
    if (_static)
    {
        App->GetModule<ModuleScene>()->GetLoadedScene()->RemoveStaticGO(this);
    }
    else
    {
        App->GetModule<ModuleScene>()->GetLoadedScene()->RemoveDynamicGO(this);
    }
    _children.clear();
    _components.clear();
}

void GameObject::SetStatic(bool isStatic)
{
    _static = isStatic;
    if (_static)
    {
        App->GetModule<ModuleScene>()->GetLoadedScene()->RemoveDynamicGO(this);
        App->GetModule<ModuleScene>()->GetLoadedScene()->AddStaticGO(this);
    }
    else
    {
        App->GetModule<ModuleScene>()->GetLoadedScene()->RemoveStaticGO(this);
        App->GetModule<ModuleScene>()->GetLoadedScene()->AddDynamicGO(this);
    }
    for (auto& child : _children)
    {
        child->SetStatic(_static);
    }
}

bool GameObject::RemoveComponent(const Component* component)
{
    auto componentIt = std::ranges::find_if(_components,
        [component](std::unique_ptr<Component>& comp)
        {
            return comp.get() == component;
        });

    if (componentIt != _components.end())
    {
        return false;
    }

    _components.erase(componentIt);
    return true;
}

void GameObject::LinkChild(GameObject* child)
{
    assert(child);

    if (!IsChild(child))
    {
        child->_parent = this;
        child->_active = IsActive();

        CHIRON_TODO("Fix transforms with parent");

        _children.push_back(std::unique_ptr<GameObject>(child));
    }
}

GameObject* GameObject::UnLinkChild(GameObject* child)
{
    assert(child);

    if (IsChild(child))
    {
        auto childIt = std::ranges::find_if(_children,
            [child](std::unique_ptr<GameObject>& actualChild)
            {
                return actualChild.get() == child;
            });
        
        auto orphan = childIt->release();
        _children.erase(childIt);

        return orphan;
    }

    return nullptr;
}

bool GameObject::IsChild(GameObject* child)
{
    return std::ranges::any_of(_children.begin(), _children.end(),
        [child](std::unique_ptr<GameObject>& actualChild)
        {
            return actualChild.get() == child;
        });
}

Component* GameObject::CreateComponent(ComponentType type)
{
    Component* newComponent = nullptr;
    switch (type)
    {
    case ComponentType::TRANSFORM:
        newComponent = new TransformComponent(this);
        break;
    case ComponentType::MESH_RENDERER:
        break;
    }

    AddComponent(newComponent);

    return newComponent;
}

bool GameObject::RemoveComponent(Component* deleteComponent)
{
    auto newEnd = std::remove_if(_components.begin(), _components.end(),
        [&deleteComponent](const std::unique_ptr<Component>& component)
        {
            return component.get() == deleteComponent;
        });
    
    if (newEnd == _components.end())
    {
        return false;
    }

    _components.erase(newEnd, _components.end());
    return true;
}

void GameObject::CopyComponent(Component* copyComponent)
{
    Component* newComponent = nullptr;
    switch (copyComponent->GetType())
    {
    case ComponentType::TRANSFORM:
        newComponent = new TransformComponent(static_cast<TransformComponent&>(*copyComponent));
        break;
    case ComponentType::MESH_RENDERER:

        break;
    }

    AddComponent(newComponent);
}

void GameObject::AddComponent(Component* newComponent)
{
    if (newComponent)
    {
        Updatable* updatable = dynamic_cast<Updatable*>(newComponent);
        if (updatable)
        {
            App->GetModule<ModuleScene>()->GetLoadedScene()->AddUpdatableComponent(updatable);
        }
        Drawable* drawable = dynamic_cast<Drawable*>(newComponent);
        if (drawable)
        {
            App->GetModule<ModuleScene>()->GetLoadedScene()->AddDrawableComponent(drawable);
        }
        newComponent->SetOwner(this);
        _components.push_back(std::unique_ptr<Component>(newComponent));
    }
}