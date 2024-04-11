#pragma once
#include "Module.h"

class ModuleInput : public Module
{
public:
    ModuleInput();
    ~ModuleInput() override;

    bool Init() override;
    UpdateStatus PreUpdate() override;
    UpdateStatus Update() override;
    bool CleanUp() override;
};