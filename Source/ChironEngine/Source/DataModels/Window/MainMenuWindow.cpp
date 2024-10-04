#include "Pch.h"
#include "MainMenuWindow.h"

#include "Application.h"

#include "Modules/ModuleWindow.h"

#include "AboutWindow.h"

#include <ImGui/imgui.h>

MainMenuWindow::MainMenuWindow(AboutWindow* aboutWindow) : Window("Main Menu"), _aboutWindow(aboutWindow)
{
}

void MainMenuWindow::Draw(const std::shared_ptr<CommandList>& commandList)
{
    if (ImGui::BeginMainMenuBar())
    {
        DrawFileMenu();
        DrawViewMenu();
        DrawHelpMenu();
    }
    ImGui::EndMainMenuBar();
}

void MainMenuWindow::DrawFileMenu()
{
    if (ImGui::BeginMenu("File"))
    {
        if (ImGui::MenuItem("Exit"))
        {
            ::PostMessage(App->GetModule<ModuleWindow>()->GetWindowId(), WM_CLOSE, 0, 0);
        }
        ImGui::EndMenu();
    }
}

void MainMenuWindow::DrawViewMenu()
{
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