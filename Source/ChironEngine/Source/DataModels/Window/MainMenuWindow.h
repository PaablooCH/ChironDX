#pragma once
#include "Window.h"

class MainMenuWindow : public Window
{
public:
    MainMenuWindow();

    void Draw(bool& enabled = defaultEnabled) override;

    static const std::string repositoryLink;

private:
    void DrawHelpMenu();

private:
    static bool defaultEnabled;

    bool _showAbout;
};
