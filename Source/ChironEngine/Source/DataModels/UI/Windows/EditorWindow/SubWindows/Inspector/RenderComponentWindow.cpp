#include "Pch.h"
#include "RenderComponentWindow.h"

#include "DataModels/UI/UiIncludes.h"

#include "Application.h"

#include "Modules/ModuleResources.h"

#include "DataModels/Components/MeshRendererComponent.h"

#include "DataModels/Assets/MaterialAsset.h"
#include "DataModels/Assets/TextureAsset.h"
#include "DataModels/DX12/CommandList/CommandList.h"
#include "DataModels/DX12/Resource/Texture.h"

#include <sstream>

RenderComponentWindow::~RenderComponentWindow()
{
}

RenderComponentWindow::RenderComponentWindow(MeshRendererComponent* component) :
    ComponentWindow(ICON_FA_DROPLET " Material", component), _assetPicker()
{
}

void RenderComponentWindow::DrawWindowContent(const std::shared_ptr<CommandList>& commandList)
{
    auto meshComponent = static_cast<MeshRendererComponent*>(_component);
    if (meshComponent)
    {
        DrawRenderWindow(commandList);
    }
}

void RenderComponentWindow::DrawRenderWindow(const std::shared_ptr<CommandList>& commandList)
{
    auto meshRenderer = static_cast<MeshRendererComponent*>(_component);
    auto materialAsset = meshRenderer->GetMaterial();

    UID actualUID = materialAsset ? materialAsset->GetUID() : 0;
    if (_assetPicker.Draw(FileType::Material, actualUID, "##matInput"))
    {
        if (actualUID == 0)
        {
            meshRenderer->SetMaterial(nullptr);
            materialAsset = nullptr;
        }
        else
        {
            meshRenderer->SetMaterial(App->GetModule<ModuleResources>()->SearchAsset<MaterialAsset>(actualUID).get());
            materialAsset = meshRenderer->GetMaterial();
        }
    }

    ImGui::SameLine();

    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 8.f);
    std::string name = materialAsset ? materialAsset->GetName() : "";
    ImGui::InputText("##matInput", &name, ImGuiInputTextFlags_ReadOnly);
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DRAGDROP_MATERIAL"))
        {
            UID draggedUIDTexture = *static_cast<UID*>(payload->Data);
            meshRenderer->SetMaterial(App->GetModule<ModuleResources>()->SearchAsset<MaterialAsset>(draggedUIDTexture).get());
            materialAsset = meshRenderer->GetMaterial();
        }
        ImGui::EndDragDropTarget();
    }

    if (!materialAsset)
    {
        return;
    }

    // ------------- COLORS ----------------------

    if (ImGui::BeginTable("###MaterialTableColors", 2))
    {
        ImGui::TableSetupColumn("###MaterialTableColorsFirstCol", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("###MaterialTableColorsSecondCol", ImGuiTableColumnFlags_WidthStretch);

        auto baseColor = materialAsset->GetBaseColor().ToVector4();
        auto specularColor = materialAsset->GetSpecularColor().ToVector4();

        ImGui::TableNextColumn();
        ImGui::Dummy(ImVec2(0.f, 0.1f));
        ImGui::Text("Base Color");

        ImGui::TableNextColumn();
        float itemWidth = (ImGui::GetContentRegionAvail().x - 8.f);
        ImGui::PushItemWidth(itemWidth);
        if (ImGui::ColorEdit4("###BaseColor", (float*)&baseColor, ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaBar))
        {
            Color newColor = Color(baseColor);
            materialAsset->SetBaseColor(newColor);
        }

        ImGui::TableNextColumn();
        ImGui::Dummy(ImVec2(0.f, 0.1f));
        ImGui::Text("Specular Color");

        ImGui::TableNextColumn();
        ImGui::PushItemWidth(itemWidth);
        if (ImGui::ColorEdit4("###SpecularColor", (float*)&specularColor, ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaBar))
        {
            Color newColor = Color(specularColor);
            materialAsset->SetSpecularColor(newColor);
        }

        ImGui::EndTable();
    }

    ImGui::Dummy(ImVec2(0.f, 1.f));
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0.f, 1.f));

    // ------------- BASE MAP ----------------------

    DrawTexture(commandList, TextureType::ALBEDO, "Base Map");
    
    DrawTexture(commandList, TextureType::METALLIC, "Property Map");
}

void RenderComponentWindow::DrawTexture(const std::shared_ptr<CommandList>& commandList, TextureType textureType, const char* label)
{
    auto materialAsset = static_cast<MeshRendererComponent*>(_component)->GetMaterial();
    auto textureAsset = materialAsset->GetTexture(textureType);
    if (ImGui::CollapsingHeader(label, ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Bullet))
    {
        ImGui::Dummy(ImVec2(0.f, 1.f));

        auto texture = textureAsset ? textureAsset->GetTexture() : nullptr;
        if (texture)
        {
            commandList->TransitionBarrier(texture->GetResource(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
            ImGui::Image((ImTextureID)(texture->GetShaderResourceView().GetGPUDescriptorHandle().ptr),
                ImVec2(20.f, 20.f), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), _secondaryColor);
            if (ImGui::BeginItemTooltip())
            {
                ImGui::Image((ImTextureID)(texture->GetShaderResourceView().GetGPUDescriptorHandle().ptr),
                    ImVec2(64, 64), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), _secondaryColor);
                ImGui::EndTooltip();
            }
        }
        else
        {
            ImGui::ColorButton("##NoTexture", ImVec4(0.2f, 0.2f, 0.2f, 1.f), ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip, ImVec2(20.f, 20.f));
        }
        ImGui::SameLine();

        UID actualUID = textureAsset ? textureAsset->GetUID() : 0;
        if (_assetPicker.Draw(FileType::Texture, actualUID, label))
        {
            if (actualUID == 0)
            {
                materialAsset->SetTexture(nullptr, textureType);
                textureAsset = nullptr;
                texture = nullptr;
            }
            else
            {
                materialAsset->SetTexture(App->GetModule<ModuleResources>()->SearchAsset<TextureAsset>(actualUID).get(), textureType);
                textureAsset = materialAsset->GetTexture(textureType);
                texture = textureAsset ? textureAsset->GetTexture() : nullptr;
            }
        }

        ImGui::SameLine();
        
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 8.f);
        std::string name = textureAsset ? textureAsset->GetName() : "";
        std::ostringstream id;
        id << "##" << label;
        ImGui::InputText(id.str().c_str(), &name, ImGuiInputTextFlags_ReadOnly);
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DRAGDROP_TEXTURE"))
            {
                UID draggedUIDTexture = *static_cast<UID*>(payload->Data);
                materialAsset->SetTexture(App->GetModule<ModuleResources>()->SearchAsset<TextureAsset>(draggedUIDTexture).get(), textureType);
                textureAsset = materialAsset->GetTexture(textureType);
                texture = textureAsset ? textureAsset->GetTexture() : nullptr;
            }
            ImGui::EndDragDropTarget();
        }

        if (texture)
        {
            ImGui::SeparatorText("Info");
            CD3DX12_RESOURCE_DESC textureDesc(texture->GetResource()->GetDesc());
            if (ImGui::BeginTable("###BaseMapInfo", 2))
            {
                ImGui::TableNextColumn();
                ImGui::Text("Path:");

                ImGui::TableNextColumn();
                ImGui::TextColored(_secondaryColor, texture->GetName().c_str());

                ImGui::TableNextColumn();
                ImGui::Text("Size:");

                ImGui::TableNextColumn();
                std::string size = std::to_string(textureDesc.Width) + " x " + std::to_string(textureDesc.Height);
                ImGui::TextColored(_secondaryColor, size.c_str());

                ImGui::TableNextColumn();
                ImGui::Text("Mipmaps:");

                ImGui::TableNextColumn();
                ImGui::TextColored(_secondaryColor, std::to_string(textureDesc.MipLevels).c_str());

                ImGui::TableNextColumn();
                ImGui::Text("sRGB (Color Texture):");

                ImGui::TableNextColumn();
                auto format = textureDesc.Format;
                if (Texture::IsSRGBFormat(format))
                {
                    ImGui::TextColored(_secondaryColor, "Yes");
                }
                else
                {
                    ImGui::TextColored(_secondaryColor, "No");
                }

                ImGui::EndTable();
            }
        }
    }
}
