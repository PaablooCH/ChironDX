#include "Pch.h"
#include "AssetPicker.h"

#include "Application.h"

#include "Modules/ModuleAssets.h"

#include "DataModels/UI/UiIncludes.h"

#include "DataModels/Assets/TextureAsset.h"

#include "DataModels/DX12/Resource/Texture.h"

#include "DataModels/FileSystem/FileSystemEntry/File/File.h"
#include "DataModels/FileSystem/FileSystemEntry/Folder/Folder.h"

#include <sstream>

AssetPicker::AssetPicker() : _selectedFileType(FileType::UNKNOWN)
{
    _rootFolder = App->GetModule<ModuleAssets>()->GetRootFolder();
}

bool AssetPicker::Draw(FileType type, UID& actualUID, const std::string& id)
{
    std::ostringstream button;
    button << ICON_FA_CIRCLE_DOT << "##" << id;

    std::ostringstream popup;
    popup << "Asset Picker" << "##" << id;

    auto text = ("Select " + FileTypeUtils::ToString(type));
    if (ImGui::Button(button.str().c_str()))
    {
        ImGui::OpenPopup(popup.str().c_str());
    }
    if (ImGui::IsItemHovered())
    {
        if (ImGui::BeginItemTooltip())
        {
            ImGui::Text(text.c_str());
            ImGui::EndTooltip();
        }
    }

    ImGui::SetNextWindowSize(ImVec2(360, 235), ImGuiCond_Appearing);
    if (ImGui::BeginPopup(popup.str().c_str()))
    {
        ImGui::Text(text.c_str());
        ImGui::Dummy(ImVec2(0.f, 1.f));
        ImVec2 gridSize = ImGui::GetContentRegionAvail();
        float xspace = ImGui::GetContentRegionAvail().x;
        ImGui::BeginChild("PickAsset", gridSize, false);

        // int itemsPerRow = std::max(1, static_cast<int>(xspace / 64.f) - 1);
        int itemsPerRow = 4;
        int index = 1;

        const int maxChars = 9;
        ImGui::Dummy(ImVec2(0.f, 1.f));

        bool retFlag;
        bool retVal = DrawDeleteAsset(actualUID, retFlag);
        if (retFlag)
        {
            return retVal;
        }

        std::queue<Folder*> foldersToCheck;
        foldersToCheck.push(_rootFolder);
        while (!foldersToCheck.empty())
        {
            auto currentFolder = foldersToCheck.front();
            foldersToCheck.pop();
            for (auto& subfolder : currentFolder->GetSubdirectories())
            {
                foldersToCheck.push(subfolder.get());
            }
            for (auto& file : currentFolder->GetFiles())
            {
                if (file->GetType() != type)
                {
                    continue;
                }
                if (index % itemsPerRow != 0)
                {
                    ImGui::SameLine();
                }
                else
                {
                    ImGui::Dummy(ImVec2(1.f, 0.f));
                    ImGui::SameLine();
                }

                ImGui::BeginGroup();
                ImGui::PushID(index);

                // Background color
                if (actualUID == file->GetUID())
                {
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(41.f / 255.f, 107.f / 255.f, 84.f / 255.f, 1.f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(59.f / 255.f, 186.f / 255.f, 115.f / 255.f, 1.f));
                }
                else
                {
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.20f, 0.20f, 0.20f, 1.0f));
                }

                switch (type)
                {
                case FileType::Material:
                    if (ImGui::Button(ICON_FA_DROPLET, ImVec2(64.f, 64.f)))
                    {
                        actualUID = file->GetUID();
                        EndImGui();
                        return true;
                    }
                    break;
                case FileType::Mesh:
                    if (ImGui::Button(ICON_FA_VECTOR_SQUARE, ImVec2(64.f, 64.f)))
                    {
                        actualUID = file->GetUID();
                        EndImGui();
                        return true;
                    }
                    break;
                case FileType::Model:
                    break;
                case FileType::Texture:
                    if (ImGui::ImageButton("", (ImTextureID)file->GetIcon()->GetTexture()->GetShaderResourceView().GetGPUDescriptorHandle().ptr,
                        ImVec2(64.f, 64.f)))
                    {
                        actualUID = file->GetUID();
                        EndImGui();
                        return true;
                    }
                    break;
                case FileType::Scene:
                    break;
                case FileType::UNKNOWN:
                    break;
                default:
                    break;
                }
                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                {
                    ImGui::CloseCurrentPopup();
                    EndImGui();
                    return true;
                }

                ImGui::PopStyleColor();
                ImGui::PopStyleColor();

                std::string name = file->GetName();

                if (name.length() > maxChars) {
                    name = name.substr(0, maxChars - 3) + "...";
                }

                ImGui::TextUnformatted(name.c_str());
                if (ImGui::BeginItemTooltip())
                {
                    ImGui::Text(file->GetPath().c_str());
                    ImGui::EndTooltip();
                }

                ImGui::PopID();
                ImGui::EndGroup();

                index++;
            }
        }
        ImGui::EndChild();
        ImGui::EndPopup();

    }
    return false;
}

bool AssetPicker::DrawDeleteAsset(UID& actualUID, bool& retFlag)
{
    retFlag = true;
    ImGui::Dummy(ImVec2(1.f, 0.f));
    ImGui::SameLine();
    ImGui::BeginGroup();
    ImGui::PushID(0);
    // Background color
    if (actualUID == 0)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(41.f / 255.f, 107.f / 255.f, 84.f / 255.f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(59.f / 255.f, 186.f / 255.f, 115.f / 255.f, 1.f));
    }
    else
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.20f, 0.20f, 0.20f, 1.0f));
    }

    if (ImGui::Button(ICON_FA_XMARK, ImVec2(64.f, 64.f)))
    {
        actualUID = 0;
        EndImGui();
        return true;
    }
    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
    {
        ImGui::CloseCurrentPopup();
        EndImGui();
        return true;
    }
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();

    ImGui::TextUnformatted("None");
    if (ImGui::BeginItemTooltip())
    {
        ImGui::Text("None");
        ImGui::EndTooltip();
    }

    ImGui::PopID();
    ImGui::EndGroup();

    ImGui::SameLine();
    retFlag = false;
    return {};
}

void AssetPicker::EndImGui()
{
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::PopID();
    ImGui::EndGroup();
    ImGui::EndChild();
    ImGui::EndPopup();
}
