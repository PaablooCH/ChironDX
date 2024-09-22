#pragma once
#include "Module.h"

class ModelAsset;
class DebugDrawPass;

class ModuleRender : public Module
{
public:
    ModuleRender();
    ~ModuleRender();

    bool Init() override;
    UpdateStatus PreUpdate() override;
    UpdateStatus Update() override;
    UpdateStatus PostUpdate() override;
    bool CleanUp() override;

    std::shared_ptr<ModelAsset> model;
private:
    std::unique_ptr<DebugDrawPass> _debugDraw;


    D3D12_RECT _scissor;
};