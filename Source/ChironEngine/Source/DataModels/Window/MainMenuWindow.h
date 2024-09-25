#pragma once
#include "Window.h"

class AboutWindow;

class MainMenuWindow : public Window
{
public:
    MainMenuWindow(AboutWindow* aboutWindow);

    static const std::string repositoryLink;
    void Draw(const std::shared_ptr<CommandList>& commandList = nullptr) override;

private:
    void DrawHelpMenu();

private:
    AboutWindow* _aboutWindow;
};
