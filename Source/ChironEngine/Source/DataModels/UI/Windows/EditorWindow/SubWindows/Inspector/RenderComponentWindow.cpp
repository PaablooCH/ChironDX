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

RenderComponentWindow::~RenderComponentWindow()
{
}

RenderComponentWindow::RenderComponentWindow(MeshRendererComponent* component) :
    ComponentWindow(ICON_FA_DROPLET " Material", component)
{
    a = App->GetModule<ModuleResources>()->RequestAsset<TextureAsset>("Assets/Models/Baker House/Baker_house.png").get();
}

void RenderComponentWindow::DrawWindowContent(const std::shared_ptr<CommandList>& commandList)
{
    auto meshComponent = static_cast<MeshRendererComponent*>(_component);
    if (meshComponent && meshComponent->GetMaterial())
    {
        DrawRenderWindow(commandList);
    }
}

void RenderComponentWindow::DrawRenderWindow(const std::shared_ptr<CommandList>& commandList)
{
    auto materialAsset = static_cast<MeshRendererComponent*>(_component)->GetMaterial();

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

    //TextureAsset* baseMap = materialAsset->GetBaseTexture();
    TextureAsset* baseMap = a.get();
    if (ImGui::CollapsingHeader("Base Map", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Bullet))
    {
        ImGui::Dummy(ImVec2(0.f, 1.f));

        if (baseMap)
        {
            auto texture = baseMap->GetTexture();
            commandList->TransitionBarrier(texture->GetResource(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
            ImGui::Image((ImTextureID)(texture->GetShaderResourceView().GetGPUDescriptorHandle().ptr),
                ImVec2(90.f, 90.f), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), _secondaryColor);
            
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

    //TextureAsset* propertyMap = materialAsset->GetPropertyTexture();
    TextureAsset* propertyMap = a.get();
    if (ImGui::CollapsingHeader("Property Map", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Bullet))
    {
        ImGui::Dummy(ImVec2(0.f, 1.f));

        if (propertyMap)
        {
            auto texture = propertyMap->GetTexture();
            commandList->TransitionBarrier(texture->GetResource(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
            ImGui::Image((ImTextureID)(texture->GetShaderResourceView().GetGPUDescriptorHandle().ptr),
                ImVec2(90.f, 90.f), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), _secondaryColor);

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
