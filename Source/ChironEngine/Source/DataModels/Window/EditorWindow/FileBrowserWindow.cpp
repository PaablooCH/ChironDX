#include "Pch.h"
#include "FileBrowserWindow.h"

#include "Application.h"

#include "Modules/ModuleFileSystem.h"
#include "Modules/ModuleScene.h"
#include "Modules/ModuleResources.h"

#include "DataModels/Assets/TextureAsset.h"
#include "DataModels/FileSystem/FileSystemEntry/Folder/Folder.h"
#include "DataModels/FileSystem/FileSystemEntry/File/File.h"

#include "DataModels/DX12/CommandList/CommandList.h"
#include "DataModels/DX12/DescriptorAllocator/DescriptorAllocator.h"
#include "DataModels/DX12/Resource/Texture.h"

#include <sstream>

FileBrowserWindow::FileBrowserWindow() : EditorWindow(ICON_FA_FOLDER_TREE " File Browser", ImGuiWindowFlags_AlwaysAutoResize),
_currentPath("Assets")
{
    _rootFolder = std::make_unique<Folder>(_currentPath);
    SelectFolder(_rootFolder.get());
    GenerateFolders();
}

FileBrowserWindow::~FileBrowserWindow()
{
}

void FileBrowserWindow::DrawWindowContent(const std::shared_ptr<CommandList>& commandList)
{
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
    stack.push({ _rootFolder.get(), false});

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
        if (folder == _rootFolder.get())
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
                std::string newPath = folder->GetPath() + "New Folder";
                if (ModuleFileSystem::CreateUniqueDirectory(newPath))
                {
                    new Folder(newPath, folder);
                }
            }
            if (IsDeletable(folder) && DrawDeleteFolderMenu(folder))
            {
                ImGui::EndPopup();
                ImGui::PopID();
                if (nodeOpen)
                {
                    ImGui::TreePop();
                }
                continue;
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
                auto draggedFileSystemEntry = _rootFolder->FindFolder(draggedUIDFileSystemEntry);
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
    ImGui::Separator();
    if (ImGui::MenuItem("Delete Folder"))
    {
        auto parentFolder = folder->GetParent();
        if (folder == _selectedFolder)
        {
            _selectedFolder = parentFolder;
        }
        ModuleFileSystem::DeleteDirectory(folder->GetPath().c_str());
        delete parentFolder->UnlinkSubdirectory(folder);
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
                    auto draggedFileSystemEntry = _rootFolder->FindFolder(draggedUIDFileSystemEntry);
                    if (draggedFileSystemEntry)
                    {
                        draggedFileSystemEntry->ChangeParent(folder.get());
                    }
                }
                ImGui::EndDragDropTarget();
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
            ImGui::PushID(file->GetUID());

            ImGui::TableNextRow();
            ImGui::TableNextColumn();

            // NAME
            std::string label;
            switch (file->GetType())
            {
            case FileType::MATERIAL:
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

            case FileType::MODEL:
                label = std::string(ICON_FA_PERSON) + " " + file->GetName();
                if (ImGui::Selectable(label.c_str(), false, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowDoubleClick))
                {
                    if (ImGui::IsMouseDoubleClicked(0))
                    {
                        App->GetModule<ModuleScene>()->ModelToGameObject(file->GetPath());
                    }
                }
                break;

            case FileType::SCENE:
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

            case FileType::TEXTURE:
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

            case FileType::MESH:
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

void FileBrowserWindow::GenerateFolders()
{
    std::vector<std::string> filesInLibPath = ModuleFileSystem::ListFilesWithPath((_currentPath + '/').c_str());
    std::queue<std::pair<std::string, Folder*>> filesToCheck;
    for (int i = 0; i < filesInLibPath.size(); i++)
    {
        filesToCheck.emplace(filesInLibPath[i], _rootFolder.get());
    }

    while (!filesToCheck.empty())
    {
        auto& pair = filesToCheck.front();
        std::string path = pair.first;
        filesToCheck.pop();
        if (ModuleFileSystem::IsDirectory(path.c_str()))
        {
            Folder* folder = new Folder(path, pair.second);
            path += "/";
            std::vector<std::string> filesInsideDirectory = ModuleFileSystem::ListFilesWithPath(path.c_str());

            for (const auto& file : filesInsideDirectory)
            {
                filesToCheck.emplace(file, folder);
            }
        }
        else if (ModuleFileSystem::GetFileExtension(path.c_str()) != META_EXT)
        {
            new File(ModuleFileSystem::GetFile(path.c_str()), pair.second);
        }
    }
}

void FileBrowserWindow::SelectFolder(Folder* folder)
{
    _selectedFolder = folder;
    _selectedFolder->SetOpened();
    _selectablePaths = ModuleFileSystem::SplitPath(_selectedFolder->GetPath());
}