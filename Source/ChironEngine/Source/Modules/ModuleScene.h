#pragma once
#include "Module.h"

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

private:

private:
    std::unique_ptr<Scene> _loadedScene;
};

