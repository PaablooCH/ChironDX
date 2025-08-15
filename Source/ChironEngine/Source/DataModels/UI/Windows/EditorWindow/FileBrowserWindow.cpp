#include "Pch.h"
#include "FileBrowserWindow.h"

#include "Application.h"

#include "Modules/ModuleAssets.h"
#include "Modules/ModuleFileSystem.h"
#include "Modules/ModuleScene.h"

#include "DataModels/Assets/TextureAsset.h"
#include "DataModels/FileSystem/FileSystemEntry/File/File.h"
#include "DataModels/FileSystem/FileSystemEntry/Folder/Folder.h"

#include "DataModels/DX12/CommandList/CommandList.h"
#include "DataModels/DX12/Resource/Texture.h"

#include <sstream>

FileBrowserWindow::FileBrowserWindow() : EditorWindow(ICON_FA_FOLDER_TREE " File Browser", ImGuiWindowFlags_AlwaysAutoResize)
{
    _rootFolder = App->GetModule<ModuleAssets>()->GetRootFolder();
    SelectFolder(_rootFolder);
}

FileBrowserWindow::~FileBrowserWindow()
{
}

const std::string& FileBrowserWindow::GetSelectedPath() const
{
    return _selectedFolder->GetPath();
}

    static bool open = true;
void FileBrowserWindow::SelectFile()
{
    ImGui::SetNextWindowSize(ImVec2(400, 200), ImGuiCond_Once);
    ImGui::Begin("Select Texture", &open, ImGuiWindowFlags_NoCollapse); // o BeginPopupModal si quieres bloqueo
    static std::string selectedTexturePath = "";

    // Parte superior: barra de búsqueda + slider + contador
    //static char searchBuffer[128] = "";
    //float totalLength = ImGui::GetContentRegionAvail().x;
    //ImGui::SetNextItemWidth(totalLength * 2 / 4);
    //ImGui::InputTextWithHint("##search", "Search...", searchBuffer, IM_ARRAYSIZE(searchBuffer));
    //ImGui::SameLine();

    //// Tamaño de miniatura
    //static float thumbnailSize = 64.0f;
    //ImGui::SetNextItemWidth(totalLength * 1 / 4);
    //ImGui::SliderFloat("##thumbsize", &thumbnailSize, 32.0f, 128.0f, "%.2f");
    //ImGui::SameLine();

    //// Contador de elementos
    //ImGui::SetNextItemWidth(totalLength * 1 / 4);
    //ImGui::Text(std::to_string((int)_selectedFolder->GetFiles().size()).c_str());

    //ImGui::Separator();
    ImVec2 gridSize = ImGui::GetContentRegionAvail();
    float xspace = ImGui::GetContentRegionAvail().x - 16;
    ImGui::BeginChild("ThumbnailGrid", gridSize, false, ImGuiWindowFlags_AlwaysVerticalScrollbar);
    
    // Cálculo de columnas dinámicas
    float padding = 10.0f;
    //int itemsPerRow = std::max(1, (int)((gridSize.x + padding) / (thumbnailSize + padding)));
    int itemsPerRow = std::max(1, static_cast<int>(xspace / 64.f));
    int index = 0;

    const int maxChars = 9;
    ImGui::Dummy(ImVec2(0.f, 1.f));
    for (const auto& tex : _selectedFolder->GetFiles()) 
    {
        // Filtrado por nombre
        /*if (strstr(tex->GetName().c_str(), searchBuffer) == nullptr)
        {
            continue;
        }*/

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

        // Fondo resaltado si está seleccionado
        if (selectedTexturePath == tex->GetPath())
        {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.4f, 0.8f, 1.0f));
        }
        else
        {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
        }

        //if (ImGui::ImageButton("", (ImTextureID)tex->GetIcon()->GetTexture()->GetShaderResourceView().GetGPUDescriptorHandle().ptr, ImVec2(thumbnailSize, thumbnailSize)))
        if (ImGui::ImageButton("", (ImTextureID)tex->GetIcon()->GetTexture()->GetShaderResourceView().GetGPUDescriptorHandle().ptr, ImVec2(64.f, 64.f)))
        {
            selectedTexturePath = tex->GetPath();

            ImGui::PopStyleColor();
            ImGui::PopID();
            ImGui::EndGroup();
            ImGui::EndChild();

            ImGui::End();
            open = false;
            return;
        }

        ImGui::PopStyleColor();

        std::string name = tex->GetName();

        if (name.length() > maxChars) {
            name = name.substr(0, maxChars - 3) + "...";
        }

        ImGui::TextUnformatted(name.c_str());
        if (ImGui::BeginItemTooltip())
        {
            ImGui::Text(tex->GetName().c_str());
            ImGui::EndTooltip();
        }

        ImGui::PopID();
        ImGui::EndGroup();

        index++;
    }

    ImGui::EndChild();

    ImGui::End();









    //if (ImGui::BeginPopup("a"))
    //{
    //    const float itemSize = 50.0f; // Tamaño del botón/cuadro de cada recurso
    //    const float padding = 10.0f;   // Espacio entre elementos

    //    // Calcula cuántos caben por fila
    //    float contentWidth = ImGui::GetContentRegionAvail().x;
    //    int itemsPerRow = std::max(1, (int)((contentWidth + padding) / (itemSize + padding)));

    //    // Tamaño de la zona con scroll (puedes ajustarlo)
    //    ImVec2 scrollAreaSize = ImVec2(0, 400);
    //    ImGui::BeginChild("AssetGrid", scrollAreaSize, true); // true = con borde

    //    int itemIndex = 0;
    //    for (const auto& file : _selectedFolder->GetFiles()) {
    //        // Comienza una nueva fila si es necesario
    //        if (itemIndex % itemsPerRow != 0)
    //            ImGui::SameLine();

    //        // Aquí puedes hacer un botón, imagen, etc.
    //        ImGui::PushID(itemIndex);
    //        if (ImGui::Button(file->GetName().c_str(), ImVec2(itemSize, itemSize)))
    //        {
    //            //selectedTexturePath = file->GetPath();
    //            ImGui::CloseCurrentPopup();
    //        }
    //        ImGui::PopID();

    //        itemIndex++;
    //    }

    //    ImGui::EndChild();

    //    ImGui::EndPopup();
    //}
}

void FileBrowserWindow::SelectFile2()
{
    if (ImGui::Button(ICON_FA_CIRCLE_DOT))
    {
        ImGui::OpenPopup("Asset Picker");
    }
    if (ImGui::IsItemHovered())
    {
        if (ImGui::BeginItemTooltip())
        {
            ImGui::Text("Select texture");
            ImGui::EndTooltip();
        }
    }

    static std::string selectedTexturePath = "";
    ImGui::SetNextWindowSize(ImVec2(360, 200), ImGuiCond_Appearing);
    if (ImGui::BeginPopup("Asset Picker"))
    {
        ImGui::Text("Select Asset");
        ImGui::Dummy(ImVec2(0.f, 1.f));
        ImVec2 gridSize = ImGui::GetContentRegionAvail();
        float xspace = ImGui::GetContentRegionAvail().x;
        ImGui::BeginChild("ThumbnailGrid", gridSize, false);

        //int itemsPerRow = std::max(1, static_cast<int>(xspace / 64.f) - 1);
        int itemsPerRow = 4;
        int index = 0;

        const int maxChars = 9;
        ImGui::Dummy(ImVec2(0.f, 1.f));
        for (const auto& tex : _selectedFolder->GetFiles())
        {

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

            // Fondo resaltado si está seleccionado
            if (selectedTexturePath == tex->GetPath())
            {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.4f, 0.8f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.5f, 0.9f, 1.0f));
            }
            else
            {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.20f, 0.20f, 0.20f, 1.0f));
            }

            if (ImGui::ImageButton("", (ImTextureID)tex->GetIcon()->GetTexture()->GetShaderResourceView().GetGPUDescriptorHandle().ptr,
                ImVec2(64.f, 64.f)))
            {
                selectedTexturePath = tex->GetPath();
            }
            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
            {
                ImGui::CloseCurrentPopup();
                ImGui::PopStyleColor();
                ImGui::PopStyleColor();
                ImGui::PopID();
                ImGui::EndGroup();
                ImGui::EndChild();
                ImGui::EndPopup();
                return;
            }

            ImGui::PopStyleColor();
            ImGui::PopStyleColor();

            std::string name = tex->GetName();

            if (name.length() > maxChars) {
                name = name.substr(0, maxChars - 3) + "...";
            }

            ImGui::TextUnformatted(name.c_str());
            if (ImGui::BeginItemTooltip())
            {
                ImGui::Text(tex->GetName().c_str());
                ImGui::EndTooltip();
            }

            ImGui::PopID();
            ImGui::EndGroup();

            index++;
        }

        ImGui::EndChild();
        ImGui::EndPopup();
    }
    if (ImGui::IsPopupOpen("Asset Picker"))
    {
        LOG_DEBUG("AAAAAAAA");
    }
}

void FileBrowserWindow::DrawWindowContent(const std::shared_ptr<CommandList>& commandList)
{
    //if (ImGui::Button("Prueba"))
    //{
        if (open)SelectFile();

    //}
    SelectFile2();
    if (ImGui::BeginChild("##FolderTreeChild", ImVec2(300, 0), ImGuiChildFlags_ResizeX | ImGuiChildFlags_Borders))
    {
        DrawFolderTree();
    }
    ImGui::EndChild();

    ImGui::SameLine();
    
    ImGui::BeginGroup();
    if (_selectedFolder)
    {
        if (ImGui::BeginChild("##FolderInfoChild", ImVec2(0, 0), ImGuiChildFlags_Borders))
        {
            DrawFolderPath();

            ImGui::Separator();
            
            DrawFolderContent(commandList);
        }
        ImGui::EndChild();
    }
    ImGui::EndGroup();
}

void FileBrowserWindow::DrawFolderTree()
{
    std::stack<std::pair<Folder*, bool>> stack;
    stack.push({ _rootFolder, false});

    while (!stack.empty())
    {
        auto& [folder, childrenVisited] = stack.top();
        stack.pop();

        if (childrenVisited)
        {
            ImGui::TreePop();
            continue;
        }

        std::ostringstream oss;
        std::string iconFolder = folder->GetOpen() ? ICON_FA_FOLDER_OPEN : ICON_FA_FOLDER;
        oss << iconFolder << " " << folder->GetName().c_str() << "###" << folder->GetUID();
        ImGui::PushID(oss.str().c_str());

        ImGuiTreeNodeFlags treeFlags = ImGuiTreeNodeFlags_OpenOnArrow;
        if (folder == _selectedFolder)
        {
            treeFlags |= ImGuiTreeNodeFlags_Selected;
        }
        if (!folder->HasSubdirectories())
        {
            if (folder->GetOpen())
            {
                folder->SetClosed();
            }
            treeFlags |= ImGuiTreeNodeFlags_Leaf;
        }
        if (folder == _rootFolder)
        {
            treeFlags |= ImGuiTreeNodeFlags_DefaultOpen;
        }

        if (folder->GetOpen())
        {
            ImGui::SetNextItemOpen(true);
        }

        bool nodeOpen = ImGui::TreeNodeEx(oss.str().c_str(), treeFlags);

        if (ImGui::IsItemClicked() && ImGui::IsMouseDoubleClicked(0) && folder != _selectedFolder)
        {
            SelectFolder(folder);
        }

        if (ImGui::BeginPopupContextItem("RightClickFolder", ImGuiPopupFlags_MouseButtonRight)) 
        {
            if (ImGui::MenuItem("Create Folder"))
            {
                std::string newPath = folder->GetPath() + '/' + "New Folder";
                if (ModuleFileSystem::CreateUniqueDirectory(newPath))
                {
                    new Folder(newPath, folder);
                }
            }
            if (IsDeletable(folder))
            {
                ImGui::Separator();
                if (DrawDeleteFolderMenu(folder))
                {
                    ImGui::EndPopup();
                    ImGui::PopID();
                    if (nodeOpen)
                    {
                        ImGui::TreePop();
                    }
                    continue;
                }
            }
            ImGui::EndPopup();
        }

        if (ImGui::BeginDragDropSource())
        {
            UID uid = folder->GetUID();
            ImGui::SetDragDropPayload("MOVE_FILES_&_FOLDERS", &uid, sizeof(uid));
            ImGui::Text(folder->GetName().c_str());
            ImGui::EndDragDropSource();
        }

        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MOVE_FILES_&_FOLDERS"))
            {
                UID draggedUIDFileSystemEntry = *static_cast<UID*>(payload->Data);
                auto draggedFileSystemEntry = _rootFolder->FindFileSystemEntry(draggedUIDFileSystemEntry);
                if (draggedFileSystemEntry)
                {
                    draggedFileSystemEntry->ChangeParent(folder);
                    ImGui::EndDragDropTarget();
                    ImGui::PopID();
                    if (nodeOpen)
                    {
                        ImGui::TreePop();
                    }
                    continue;
                }
            }
            ImGui::EndDragDropTarget();
        }

        if (nodeOpen)
        {
            if (!folder->GetOpen() && folder->HasSubdirectories())
            {
                folder->SetOpened();
            }
            stack.push({ folder, true });
            auto& subdirectories = folder->GetSubdirectories();
            for (int i = static_cast<int>(subdirectories.size()) - 1; i >= 0; i--)
            {
                Folder* subdirectory = subdirectories[i].get();
                stack.push({ subdirectory, false });
            }
        }
        else
        {
            if (folder->GetOpen())
            {
                folder->SetClosed();
            }
        }
        ImGui::PopID();
    }
}

bool FileBrowserWindow::DrawDeleteFolderMenu(Folder* folder)
{
    if (ImGui::MenuItem("Delete Folder"))
    {
        auto parent = folder->GetParent();
        App->GetModule<ModuleAssets>()->DeleteFolder(folder);
        if (_selectedFolder == nullptr)
        {
            SelectFolder(parent);
        }
        return true;
    }
    return false;
}

bool FileBrowserWindow::DrawDeleteFileMenu(File* file)
{
    if (ImGui::MenuItem("Delete File"))
    {
        App->GetModule<ModuleAssets>()->DeleteFileC(file);
        return true;
    }
    return false;
}

void FileBrowserWindow::DrawFolderPath()
{
    for (int i = 0; i < _selectablePaths.size(); i++)
    {
        if (ImGui::Button(_selectablePaths[i].c_str()))
        {
            if (i != _selectablePaths.size() - 1)
            {
                std::vector<std::string> extracted(
                    std::make_move_iterator(_selectablePaths.begin()),
                    std::make_move_iterator(_selectablePaths.begin() + i + 1));
                SelectFolder(_rootFolder->FindFolder(extracted));
            }
        }
        if (i < _selectablePaths.size() - 1)
        {
            ImGui::SameLine();
            DrawButtonSubdirectories(i, _selectablePaths[i + 1]);
            ImGui::SameLine();
        }
    }
}

void FileBrowserWindow::DrawButtonSubdirectories(int iterator, const std::string& actualSubdirectory)
{
    std::string popupId = "SubdirectoriesMenu##" + std::to_string(iterator);
    std::string iconFolder = ICON_FA_GREATER_THAN;
    std::ostringstream oss;
    oss << iconFolder << "##" << iterator;
    if (ImGui::Button(oss.str().c_str()))
    {
        ImGui::OpenPopup(popupId.c_str());
    }
    if (ImGui::BeginPopup(popupId.c_str()))
    {
        if (static_cast<unsigned long long>(iterator) + 1 > _selectablePaths.size())
        {
            ImGui::EndPopup();
            return;
        }
        std::vector<std::string> extracted(
            _selectablePaths.begin(),
            _selectablePaths.begin() + iterator + 1);
        auto folder = _rootFolder->FindFolder(extracted);

        auto& subdirectories = folder->GetSubdirectories();
        for (int i = 0; i < subdirectories.size(); i++)
        {
            std::string name = subdirectories[i]->GetName();
            if (name.empty())
            {
                name = "Unnamed Folder";
            }
            std::string label = name + "##" + std::to_string(i);
            bool marked = name == actualSubdirectory;
            if (ImGui::MenuItem(label.c_str(), NULL, marked))
            {
                SelectFolder(subdirectories[i].get());
            }
        }
        ImGui::EndPopup();
    }
}

void FileBrowserWindow::DrawFolderContent(const std::shared_ptr<CommandList>& commandList)
{
    ImGuiTableFlags flags =
        ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti
        | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_NoBordersInBody
        | ImGuiTableFlags_ScrollY;

    if (ImGui::BeginTable("FolderFileIconTable", 4, flags))
    {
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_DefaultSort);
        ImGui::TableSetupColumn("Date");
        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableHeadersRow();
        for (auto& folder : _selectedFolder->GetSubdirectories())
        {
            // Checking if a folder no longer exists before printing
            if (!folder)
            {
                continue;
            }
            ImGui::PushID(folder->GetUID());

            ImGui::TableNextRow();

            // NAME
            ImGui::TableNextColumn();
            std::string label = std::string(ICON_FA_FOLDER) + " " + folder->GetName();
            if (ImGui::Selectable(label.c_str(), false, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowDoubleClick))
            {
                if (ImGui::IsMouseDoubleClicked(0))
                {
                    SelectFolder(folder.get());
                }
            }
            if (ImGui::BeginDragDropSource())
            {
                UID uid = folder->GetUID();
                ImGui::SetDragDropPayload("MOVE_FILES_&_FOLDERS", &uid, sizeof(uid));
                ImGui::Text(folder->GetName().c_str());
                ImGui::EndDragDropSource();
            }

            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MOVE_FILES_&_FOLDERS"))
                {
                    UID draggedUIDFileSystemEntry = *static_cast<UID*>(payload->Data);
                    auto draggedFileSystemEntry = _rootFolder->FindFileSystemEntry(draggedUIDFileSystemEntry);
                    if (draggedFileSystemEntry)
                    {
                        draggedFileSystemEntry->ChangeParent(folder.get());
                    }
                }
                ImGui::EndDragDropTarget();
            }

            if (ImGui::BeginPopupContextItem("RightClickFolderInsideFolder", ImGuiPopupFlags_MouseButtonRight))
            {
                if (IsDeletable(folder.get()) && DrawDeleteFolderMenu(folder.get()))
                {
                    ImGui::EndPopup();
                    ImGui::PopID();
                    continue;
                }
                ImGui::EndPopup();
            }

            // DATE
            ImGui::TableNextColumn();
            ImGui::Text(folder->GetDate().c_str());

            // TYPE
            ImGui::TableNextColumn();

            // SIZE
            ImGui::TableNextColumn();

            ImGui::PopID();
        }

        for (auto& file : _selectedFolder->GetFiles())
        {
            if (!file)
            {
                continue;
            }
            ImGui::PushID(file->GetUID());

            ImGui::TableNextRow();
            ImGui::TableNextColumn();

            // NAME
            std::string label;
            switch (file->GetType())
            {
            case FileType::Material:
                label = std::string(ICON_FA_DROPLET) + " " + file->GetName();
                ImGui::Selectable(label.c_str(), false, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_DontClosePopups);
                if (ImGui::BeginDragDropSource())
                {
                    UID uid = file->GetUID();
                    ImGui::SetDragDropPayload("DRAGDROP_MATERIAL", &uid, sizeof(UID));
                    ImGui::Text(file->GetName().c_str());
                    ImGui::EndDragDropSource();
                }
                break;

            case FileType::Model:
                label = std::string(ICON_FA_PERSON) + " " + file->GetName();
                if (ImGui::Selectable(label.c_str(), false, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowDoubleClick))
                {
                    if (ImGui::IsMouseDoubleClicked(0))
                    {
                        App->GetModule<ModuleScene>()->ModelToGameObject(file->GetPath());
                        ImGui::PopID();
                        ImGui::EndTable();
                        return;
                    }
                }
                break;

            case FileType::Scene:
                label = std::string(ICON_FA_BOX_OPEN) + " " + file->GetName();
                ImGui::Text(label.c_str());
                if (ImGui::Selectable(label.c_str(), false, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowDoubleClick))
                {
                    if (ImGui::IsMouseDoubleClicked(0))
                    {
                        auto start = std::chrono::steady_clock::now();
                        App->GetModule<ModuleScene>()->LoadScene(file->GetPath(),
                            [start]()
                            {
                                auto end = std::chrono::steady_clock::now();
                                auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
                                LOG_INFO("Scene Loaded! Took {} seconds", static_cast<int>(duration));
                            });
                    }
                }
                break;

            case FileType::Texture:
                label = std::string(ICON_FA_PALETTE) + " " + file->GetName();
                ImGui::Selectable(label.c_str(), false, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_DontClosePopups);
                if (ImGui::BeginItemTooltip())
                {
                    commandList->TransitionBarrier(file->GetIcon()->GetTexture().get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
                    ImGui::Image((ImTextureID)(file->GetIcon()->GetTexture()->GetShaderResourceView().GetGPUDescriptorHandle().ptr),
                        ImVec2(64, 64));
                    ImGui::EndTooltip();
                }
                if (ImGui::BeginDragDropSource())
                {
                    UID uid = file->GetUID();
                    ImGui::SetDragDropPayload("DRAGDROP_TEXTURE", &uid, sizeof(UID));
                    ImGui::Text(file->GetName().c_str());
                    ImGui::EndDragDropSource();
                }
                break;

            case FileType::Mesh:
                label = std::string(ICON_FA_VECTOR_SQUARE) + " " + file->GetName();
                ImGui::Selectable(label.c_str(), false, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_DontClosePopups);
                if (ImGui::BeginDragDropSource())
                {
                    UID uid = file->GetUID();
                    ImGui::SetDragDropPayload("DRAGDROP_MESH", &uid, sizeof(UID));
                    ImGui::Text(file->GetName().c_str());
                    ImGui::EndDragDropSource();
                }
                break;

            case FileType::UNKNOWN:
                label = std::string(ICON_FA_QUESTION) + " " + file->GetName();
                ImGui::Selectable(label.c_str(), false, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_DontClosePopups);
                break;
            }

            if (ImGui::BeginDragDropSource())
            {
                UID uid = file->GetUID();
                ImGui::SetDragDropPayload("MOVE_FILES_&_FOLDERS", &uid, sizeof(uid));
                ImGui::Text(file->GetName().c_str());
                ImGui::EndDragDropSource();
            }

            if (ImGui::BeginPopupContextItem("RightClickFile", ImGuiPopupFlags_MouseButtonRight))
            {
                if (DrawDeleteFileMenu(file.get()))
                {
                    ImGui::EndPopup();
                    ImGui::PopID();
                    continue;
                }
                ImGui::EndPopup();
            }

            // DATE
            ImGui::TableNextColumn();
            ImGui::Text(file->GetDate().c_str());

            // TYPE
            ImGui::TableNextColumn();
            ImGui::Text(file->GetExt().c_str());

            // SIZE
            ImGui::TableNextColumn();
            ImGui::Text(file->GetSize().c_str());

            ImGui::PopID();
        }
        ImGui::EndTable();
    }
}

void FileBrowserWindow::SelectFolder(Folder* folder)
{
    _selectedFolder = folder;
    _selectedFolder->SetOpened();
    _selectablePaths = ModuleFileSystem::SplitPath(_selectedFolder->GetPath());
}