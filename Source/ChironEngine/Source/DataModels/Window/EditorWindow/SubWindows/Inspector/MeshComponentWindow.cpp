#include "Pch.h"
#include "MeshComponentWindow.h"

#include "DataModels/Assets/MeshAsset.h"

#include "DataModels/DX12/Resource/IndexBuffer.h"
#include "DataModels/DX12/Resource/VertexBuffer.h"

#include "DataModels/Components/MeshRendererComponent.h"

#include "DataModels/GameObject/GameObject.h"

MeshComponentWindow::~MeshComponentWindow()
{
}

MeshComponentWindow::MeshComponentWindow(MeshRendererComponent* component) :
    ComponentWindow(ICON_FA_VECTOR_SQUARE " Mesh", component)
{
}

void MeshComponentWindow::DrawWindowContent(const std::shared_ptr<CommandList>& commandList)
{
}

void MeshComponentWindow::RemoveAction()
{
}

void MeshComponentWindow::DrawMeshWindow()
{
}
