#pragma once
#include "Window.h"

class AboutWindow;

class MainMenuWindow : public Window
{
public:
    MainMenuWindow(AboutWindow* aboutWindow);

    void Draw(const std::shared_ptr<CommandList>& commandList = nullptr) override;

private:
    void DrawFileMenu();
    void DrawViewMenu();
    void DrawHelpMenu();

private:
    AboutWindow* _aboutWindow;
};
