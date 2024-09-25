#pragma once
#include "Window.h"

class MainMenuWindow : public Window
{
public:
    MainMenuWindow();


    static const std::string repositoryLink;
    void Draw(const std::shared_ptr<CommandList>& commandList = nullptr) override;

private:
    void DrawHelpMenu();

private:
    static bool defaultEnabled;

    bool _showAbout;
};
