#pragma once
#include "Module.h"

class GameObject;
class Scene;

class ModuleScene : public Module
{
public:
    ModuleScene();
    ~ModuleScene();

    bool Init() override;
    bool Start() override;
    UpdateStatus PreUpdate() override;
    UpdateStatus Update() override;
    UpdateStatus PostUpdate() override;
    bool CleanUp() override;

    // ------------- GETTERS ----------------------

    inline Scene* GetLoadedScene();
    inline GameObject* GetSelectedGameObject();

private:

private:
    std::unique_ptr<Scene> _loadedScene;
    GameObject* _selectedGameObject;
};

inline Scene* ModuleScene::GetLoadedScene()
{
    return _loadedScene.get();
}

inline GameObject* ModuleScene::GetSelectedGameObject()
{
    return _selectedGameObject;
}
