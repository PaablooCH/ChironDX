#include "Pch.h"
#include "SubWindow.h"

SubWindow::SubWindow(std::string name) : Window(name), _flags(ImGuiTreeNodeFlags_DefaultOpen)
{
}

SubWindow::~SubWindow()
{
}

void SubWindow::Draw(const std::shared_ptr<CommandList>& commandList)
{
    if (ImGui::CollapsingHeader(_name.c_str(), _flags))
    {
        DrawWindowContent(commandList);
    }
}