#pragma once
#include "Module.h"

#include "Auxiliar/BidirectionalMap.h"
#include "DataModels/FileSystem/UID/UID.h"

class FileBrowserWindow;
class Folder;
class File;

class ModuleAssets : public Module
{
public:
    ModuleAssets();
    ~ModuleAssets() override;

    bool Init() override;
    bool Start() override;
    bool CleanUp() override;

    void AddDroppedFiles(HDROP hDrop);
    UID CopyAndSaveFile(const std::string& path);
    void SaveEngineFile(const std::string& path, const void* fileBuffer, size_t size, Folder* folder);
    UID CreateMetaFileC(const std::string& filePath);

    inline void UpdatePath(const std::string& path, UID uid);
    inline void UpdateUID(UID uid, const std::string& path);

    inline bool ExistsFile(UID uid) const;

    // ------------- GETTERS ----------------------

    inline Folder* GetRootFolder() const;
    inline const std::string GetFilePath(UID uid) const;
    inline UID GetFileUID(const std::string& filePath) const;

    // ------------- FILES MANAGER ----------------------

    void DeleteFileC(File* file);

    // ------------- FOLDER MANAGER ----------------------

    void DeleteFolder(Folder* folder);

private:
    void ScanAssetFolder();
    void LoadAssetsIcons();

    // ------------- METAS FILES ----------------------

    void CreateMetaOfFile(File* file);
    void ProcessMetaFile(const std::string& path, File* file);

private:
    // Map to store the relationship between GUIDs and file paths
    Chiron::BidirectionalMap<UID, std::string> _guidToPath;

    std::unique_ptr<Folder> _rootFolder;

    FileBrowserWindow* _fileBrowserWindow;
};

inline void ModuleAssets::UpdatePath(const std::string& path, UID uid)
{
    _guidToPath.UpdateByValue(path, uid);
}

inline void ModuleAssets::UpdateUID(UID uid, const std::string& path)
{
    _guidToPath.UpdateByKey(uid, path);
}

inline bool ModuleAssets::ExistsFile(UID uid) const
{
    return _guidToPath.ContainsKey(uid);
}

inline Folder* ModuleAssets::GetRootFolder() const 
{ 
    return _rootFolder.get(); 
}

inline const std::string ModuleAssets::GetFilePath(UID uid) const
{
    std::optional<std::string> value = _guidToPath.GetValue(uid);
    if (value)
    {
        return *value;
    }
    static std::string emptyString;
    return emptyString;
}

inline UID ModuleAssets::GetFileUID(const std::string& filePath) const
{
    std::optional<UID> value = _guidToPath.GetKey(filePath);
    if (value)
    {
        return *value;
    }
    return 0;
}