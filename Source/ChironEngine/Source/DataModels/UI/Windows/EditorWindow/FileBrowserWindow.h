#pragma once
#include "EditorWindow.h"

class CommandList;
class Folder;
class File;

class FileBrowserWindow : public EditorWindow
{
public:
    FileBrowserWindow();
    ~FileBrowserWindow() override;

    // ------------- GETTERS ----------------------

    inline Folder* GetSelectedFolder() const;
    const std::string& GetSelectedPath() const;

private:
    void DrawWindowContent(const std::shared_ptr<CommandList>& commandList) override;
    void DrawFolderTree();
    bool MoveFileOrFolder(const ImGuiPayload* payload, Folder*& folder, bool nodeOpen);
    bool DrawDeleteFolderMenu(Folder* folder);
    bool DrawDeleteFileMenu(File* file);
    void DrawFolderPath();
    void DrawButtonSubdirectories(int i, const std::string& actualSubdirectory);
    void DrawFolderContent(const std::shared_ptr<CommandList>& commandList);

    void SelectFolder(Folder* folder);

    inline bool IsDeletable(Folder* folder) const;
private:
    Folder* _rootFolder;
    Folder* _selectedFolder;

    std::vector<std::string> _selectablePaths;
};

inline Folder* FileBrowserWindow::GetSelectedFolder() const 
{ 
    return _selectedFolder; 
}

inline bool FileBrowserWindow::IsDeletable(Folder* folder) const
{
    return folder != _rootFolder;
}