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

private:
    std::unique_ptr<DebugDrawPass> _debugDraw;

    std::shared_ptr<ModelAsset> model;

    D3D12_RECT _scissor;
};