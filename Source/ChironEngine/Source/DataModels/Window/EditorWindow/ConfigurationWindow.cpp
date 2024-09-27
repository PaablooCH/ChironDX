#include "Pch.h"
#include "ConfigurationWindow.h"

#include "SubWindows/EngineStatisticsSubWindow.h"

ConfigurationWindow::ConfigurationWindow() : EditorWindow("Configuration")
{
    _subWindows.push_back(std::make_unique<EngineStatisticsSubWindow>());
}

ConfigurationWindow::~ConfigurationWindow()
{
}

void ConfigurationWindow::DrawWindowContent(const std::shared_ptr<CommandList>& commandList)
{
    for (int i = 0; i < _subWindows.size(); i++)
    {
        _subWindows[i]->Draw(commandList);
        if (i < _subWindows.size() - 1)
        {
            ImGui::Separator();
        }
    }
}
