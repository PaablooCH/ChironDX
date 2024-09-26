#include "Pch.h"
#include "ConfigurationWindow.h"

#include "SubWindows/EngineStatisticsWindow.h"

ConfigurationWindow::ConfigurationWindow() : EditorWindow("Configuration")
{
    _subWindows.push_back(std::make_unique<EngineStatisticsWindow>());
}

ConfigurationWindow::~ConfigurationWindow()
{
}

void ConfigurationWindow::DrawWindowContent(const std::shared_ptr<CommandList>& commandList)
{
    for (auto& window : _subWindows)
    {
        window->Draw(commandList);
    }
}
