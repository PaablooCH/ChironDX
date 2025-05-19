#pragma once
#include "Module.h"
#include "Structs/ThemeColors.h"

class CommandList;
class MainMenuWindow;
class Window;
class DescriptorAllocation;

enum class WindowsType
{
    CONFIGURATION,
    CONSOLE,
    FILE_BROWSER,
    HIERARCHY,
    INSPECTOR,
    SCENE,
    SIZE
};

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

    inline const std::vector<std::unique_ptr<Window>>& GetWindows() const;
    inline Window* GetWindow(WindowsType wType) const;

private:
    void StartDock() const;
    void SaveWindowsState() const;
    void LoadWindowsState();

    void SetThemes();
    void ApplyTheme(const ThemeColors& theme);

    void SetStyle();

private:
    std::vector<std::unique_ptr<Window>> _windows;
    std::unique_ptr<MainMenuWindow> _mainMenu;

    ThemeColors _colorfullStyle;
    ThemeColors _minimalistStyle;
    ThemeColors _darkGreenStyle;

    std::unique_ptr<DescriptorAllocation> _srvDescHeap;

    std::shared_ptr<CommandList> _drawCommandList;

    ImGuiWindowFlags _dockFlags;

    bool _startDock;
};

inline const std::vector<std::unique_ptr<Window>>& ModuleEditor::GetWindows() const
{
    return _windows;
}

inline Window* ModuleEditor::GetWindow(WindowsType wType) const
{
    return _windows[static_cast<int>(wType)].get();
}
