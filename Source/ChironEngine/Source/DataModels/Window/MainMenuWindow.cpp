#include "Pch.h"
#include "MainMenuWindow.h"

#include "ImGui/imgui.h"

const std::string MainMenuWindow::repositoryLink = "https://github.com/PaablooCH/ChironDX";
bool MainMenuWindow::defaultEnabled = true;

MainMenuWindow::MainMenuWindow() : Window("Main Menu"), _showAbout(false)
{
}

void MainMenuWindow::Draw(bool& enabled, const std::shared_ptr<CommandList>& commandList)
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
        ImGui::MenuItem("About ChironDX", NULL, &_showAbout);
        if (ImGui::MenuItem("GitHub Link"))
        {
            ShellExecuteA(NULL, "open", repositoryLink.c_str(), NULL, NULL, SW_SHOWNORMAL);
        }
        ImGui::EndMenu();
    }
    //about->Draw(_showAbout);
}