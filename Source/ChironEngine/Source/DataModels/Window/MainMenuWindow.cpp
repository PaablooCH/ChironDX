#include "Pch.h"
#include "MainMenuWindow.h"

#include "AboutWindow.h"

#include <ImGui/imgui.h>

MainMenuWindow::MainMenuWindow(AboutWindow* aboutWindow) : Window("Main Menu"), _aboutWindow(aboutWindow)
{
}

void MainMenuWindow::Draw(const std::shared_ptr<CommandList>& commandList)
{
    if (ImGui::BeginMainMenuBar())
    {
        DrawHelpMenu();
    }
    ImGui::EndMainMenuBar();
}

void MainMenuWindow::DrawHelpMenu()
{
    if (ImGui::BeginMenu("Help"))
    {
        bool& showAbout = _aboutWindow->GetEnabled();
        ImGui::MenuItem("About", NULL, &showAbout);
        if (ImGui::MenuItem("GitHub Link"))
        {
            Chiron::Utils::OpenLink(REPOSITORY_URL);
        }
        ImGui::EndMenu();
    }
}