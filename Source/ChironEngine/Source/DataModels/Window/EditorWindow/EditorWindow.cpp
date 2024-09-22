#include "Pch.h"
#include "EditorWindow.h"

#include "DataModels/DX12/CommandList/CommandList.h"

EditorWindow::EditorWindow(const std::string& name) : Window(name), _flags(ImGuiWindowFlags_None), _focused(false)
{
}

EditorWindow::~EditorWindow()
{
}

void EditorWindow::Draw(bool& enabled, const std::shared_ptr<CommandList>& commandList)
{
    if (enabled)
    {
        if (ImGui::Begin(_name.c_str(), &enabled, _flags))
        {
            DrawWindowContent(commandList);
            _focused = ImGui::IsWindowFocused() && ImGui::IsWindowHovered();
        }
        ImGui::End();
    }
    // The call to ImGui::Begin can change the value of io_enabled
    // so using "else" will cause the window to remain focused for an extra frame
    if (!enabled)
    {
        _focused = false;
    }
}