#include "Pch.h"
#include "RenderComponentWindow.h"

#include "DataModels/Components/MeshRendererComponent.h"

#include "DataModels/GameObject/GameObject.h"

RenderComponentWindow::~RenderComponentWindow()
{
}

RenderComponentWindow::RenderComponentWindow(MeshRendererComponent* component) :
    ComponentWindow(ICON_FA_DROPLET " Material", component)
{
}

void RenderComponentWindow::DrawWindowContent(const std::shared_ptr<CommandList>& commandList)
{
}

void RenderComponentWindow::RemoveAction()
{
}

void RenderComponentWindow::DrawRenderWindow(const std::shared_ptr<CommandList>& commandList)
{
}
