#include "Pch.h"
#include "MeshComponentWindow.h"

#include "DataModels/UI/UiIncludes.h"

#include "DataModels/Assets/MeshAsset.h"

#include "DataModels/DX12/Resource/IndexBuffer.h"
#include "DataModels/DX12/Resource/VertexBuffer.h"

#include "DataModels/Components/MeshRendererComponent.h"

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
    if (meshComponent && meshComponent->GetMesh())
    {
        DrawMeshWindow();
    }
}

void MeshComponentWindow::DrawMeshWindow()
{
    auto mesh = static_cast<MeshRendererComponent*>(_component)->GetMesh();

    CHIRON_TODO("ReImport via browser");

    ImGui::Text(mesh->GetName().c_str());

    ImGui::SeparatorText("Geometry");
    if (ImGui::BeginTable("##geometryInfo", 2))
    {
        ImGui::TableNextColumn();
        ImGui::Text("Vertices: ");
        ImGui::TableNextColumn();
        std::string verticesText = std::to_string(mesh->GetVertexBuffer()->GetNumVertex());
        ImGui::TextColored(_secondaryColor, verticesText.c_str());

        ImGui::TableNextColumn();
        ImGui::Text("Indices: ");
        ImGui::TableNextColumn();
        std::string indicesText = std::to_string(mesh->GetIndexBuffer()->GetNumIndices());
        ImGui::TextColored(_secondaryColor, indicesText.c_str());

        ImGui::EndTable();
    }
}
