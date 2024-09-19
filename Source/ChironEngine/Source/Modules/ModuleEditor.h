#pragma once
#include "Module.h"

class DescriptorAllocation;

class ModuleEditor : public Module
{
public:
    ModuleEditor();
    ~ModuleEditor() override;

    bool Init() override;
    bool Start() override;
    bool CleanUp() override;

    UpdateStatus PreUpdate() override;
    UpdateStatus Update() override;
    UpdateStatus PostUpdate() override;

private:
    std::unique_ptr<DescriptorAllocation> _srvDescHeap;
};
