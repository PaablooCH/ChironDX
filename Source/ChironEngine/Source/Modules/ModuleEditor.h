#pragma once
#include "Module.h"
#include "Structs/ThemeColors.h"

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
    void SetStyles();
    void ApplyTheme(const ThemeColors& theme);

private:
    enum class WindowsType
    {
        SCENE,
        CONSOLE,
        ABOUT,
        CONFIGURATION,
        SIZE
    };

    std::vector<std::unique_ptr<Window>> _windows;
    std::unique_ptr<MainMenuWindow> _mainMenu;

    ThemeColors _colorfullStyle;
    ThemeColors _minimalistStyle;
    ThemeColors _darkGreenStyle;

    std::unique_ptr<DescriptorAllocation> _srvDescHeap;
};
