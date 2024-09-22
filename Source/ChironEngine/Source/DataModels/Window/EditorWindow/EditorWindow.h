#pragma once
#include "DataModels/Window/Window.h"

#include "ImGui/imgui.h"

class EditorWindow : public Window
{
public:
    virtual ~EditorWindow();

    void Draw(bool& enabled, const std::shared_ptr<CommandList>& commandList) override;

protected:
    EditorWindow(const std::string& name);

    virtual void DrawWindowContent(const std::shared_ptr<CommandList>& commandList) = 0;

    ImGuiWindowFlags _flags;

private:
    bool _focused;
};
