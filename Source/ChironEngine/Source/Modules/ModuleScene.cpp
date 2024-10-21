#include "Pch.h"
#include "ModuleScene.h"

#include "DataModels/Scene/Scene.h"

ModuleScene::ModuleScene() : _loadedScene(nullptr)
{
}

ModuleScene::~ModuleScene()
{
}

bool ModuleScene::Init()
{
    return true;
}

bool ModuleScene::Start()
{
    return true;
}

UpdateStatus ModuleScene::PreUpdate()
{
    return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus ModuleScene::Update()
{
    return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus ModuleScene::PostUpdate()
{
    return UpdateStatus::UPDATE_CONTINUE;
}

bool ModuleScene::CleanUp()
{
    return true;
}
