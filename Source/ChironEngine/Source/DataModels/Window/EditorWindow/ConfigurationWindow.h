#pragma once
#include "EditorWindow.h"

class ConfigurationWindow : public EditorWindow
{
    ConfigurationWindow();
    ~ConfigurationWindow() override;

private:
    void DrawWindowContent(const std::shared_ptr<CommandList>& commandList) override;
};

