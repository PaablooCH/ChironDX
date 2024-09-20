#include "Pch.h"
#include "EditorWindow.h"

EditorWindow::EditorWindow(const std::string& name) : Window(name), _flags(ImGuiWindowFlags_None), _focused(false)
{
}

EditorWindow::~EditorWindow()
{
}

void EditorWindow::Draw(bool& enabled)
{
    if (enabled)
    {
        if (ImGui::Begin(_name.c_str(), &enabled, _flags))
        {
            DrawWindowContent();
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