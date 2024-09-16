#pragma once
#include "Module.h"

class CommandList;
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
    // Indicate to the driver how a resource should be used in upcoming commands.
    // Is used once its loaded into a Queue
    std::shared_ptr<CommandList> _drawCommandList;

    std::unique_ptr<DebugDrawPass> _debugDraw;
    
    std::shared_ptr<ModelAsset> model;

    D3D12_RECT _scissor;
};