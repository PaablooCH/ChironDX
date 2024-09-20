#pragma once
#include "DataModels/Window/Window.h"

#include "ImGui/imgui.h"

class EditorWindow : public Window
{
public:
    virtual ~EditorWindow();

    void Draw(bool& enabled) override;

protected:
    EditorWindow(const std::string& name);

    virtual void DrawWindowContent() = 0;

    ImGuiWindowFlags _flags;

private:
    bool _focused;
};
