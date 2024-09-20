#pragma once
#include "Module.h"

class MainMenuWindow;
class Window;
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
    std::vector<std::unique_ptr<Window>> _windows;
    std::unique_ptr<MainMenuWindow> _mainMenu;

    std::unique_ptr<DescriptorAllocation> _srvDescHeap;
};
