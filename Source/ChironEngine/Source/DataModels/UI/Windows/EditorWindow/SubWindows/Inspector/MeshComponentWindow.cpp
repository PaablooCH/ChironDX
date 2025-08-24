#include "Pch.h"
#include "MeshComponentWindow.h"

#include "DataModels/UI/UiIncludes.h"

#include "Application.h"

#include "Modules/ModuleResources.h"

#include "DataModels/Assets/MeshAsset.h"

#include "DataModels/DX12/Resource/IndexBuffer.h"
#include "DataModels/DX12/Resource/VertexBuffer.h"

#include "DataModels/Components/MeshRendererComponent.h"

MeshComponentWindow::~MeshComponentWindow()
{
}

MeshComponentWindow::MeshComponentWindow(MeshRendererComponent* component) :
    ComponentWindow(ICON_FA_VECTOR_SQUARE " Mesh", component), _assetPicker()
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
    auto meshAsset = meshRenderer->GetMesh();

    UID actualUID = meshAsset ? meshAsset->GetUID() : 0;
    if (_assetPicker.Draw(FileType::Mesh, actualUID, "##matInput"))
    {
        if (actualUID == 0)
        {
            meshRenderer->SetMesh(nullptr);
            meshAsset = nullptr;
        }
        else
        {
            meshRenderer->SetMesh(App->GetModule<ModuleResources>()->SearchAsset<MeshAsset>(actualUID).get());
            meshAsset = meshRenderer->GetMesh();
        }
    }

    ImGui::SameLine();

    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 8.f);
    std::string name = meshAsset ? meshAsset->GetName() : "";
    ImGui::InputText("##meshInput", &name, ImGuiInputTextFlags_ReadOnly);
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DRAGDROP_MESH"))
        {
            UID draggedUIDTexture = *static_cast<UID*>(payload->Data);
            meshRenderer->SetMesh(App->GetModule<ModuleResources>()->SearchAsset<MeshAsset>(draggedUIDTexture).get());
            meshAsset = meshRenderer->GetMesh();
        }
        ImGui::EndDragDropTarget();
    }

    if (!meshAsset)
    {
        return;
    }

    ImGui::SeparatorText("Geometry");
    if (ImGui::BeginTable("##geometryInfo", 2))
    {
        ImGui::TableNextColumn();
        ImGui::Text("Vertices: ");
        ImGui::TableNextColumn();
        std::string verticesText = std::to_string(meshAsset->GetVertexBuffer()->GetNumVertex());
        ImGui::TextColored(_secondaryColor, verticesText.c_str());

        ImGui::TableNextColumn();
        ImGui::Text("Indices: ");
        ImGui::TableNextColumn();
        std::string indicesText = std::to_string(meshAsset->GetIndexBuffer()->GetNumIndices());
        ImGui::TextColored(_secondaryColor, indicesText.c_str());

        ImGui::EndTable();
    }
}
