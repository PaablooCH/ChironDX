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
    auto meshComponent = static_cast<MeshRendererComponent*>(_component);
    if (meshComponent)
    {
        DrawMeshWindow();
    }
}

void MeshComponentWindow::DrawMeshWindow()
{
    auto meshRenderer = static_cast<MeshRendererComponent*>(_component);

    CHIRON_TODO("ReImport via browser");

    ImGui::SeparatorText("Geometry");
    if (ImGui::BeginTable("##geometryInfo", 2))
    {
        ImGui::TableNextColumn();
        ImGui::Text("Vertices: ");
        ImGui::TableNextColumn();
        std::string verticesText = std::to_string(meshRenderer->GetMesh()->GetVertexBuffer()->GetNumVertex());
        ImGui::Text(verticesText.c_str());

        ImGui::TableNextColumn();
        ImGui::Text("Indices: ");
        ImGui::TableNextColumn();
        std::string indicesText = std::to_string(meshRenderer->GetMesh()->GetIndexBuffer()->GetNumIndices());
        ImGui::Text(indicesText.c_str());

        ImGui::EndTable();
    }
}
