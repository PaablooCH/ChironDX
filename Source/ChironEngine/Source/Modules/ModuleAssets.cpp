#include "Pch.h"
#include "ModuleAssets.h"

#include "Application.h"

#include "ModuleEditor.h"
#include "ModuleFileSystem.h"

#include "DataModels/FileSystem/Json/Json.h"

#include "DataModels/FileSystem/FileSystemEntry/Folder/Folder.h"
#include "DataModels/FileSystem/FileSystemEntry/File/File.h"


#include "DataModels/FileSystem/UID/UIDGenerator.h"

#include "DataModels/UI/Windows/EditorWindow/FileBrowserWindow.h"
#include "Defines/FileSystemDefine.h"

ModuleAssets::ModuleAssets() : _fileBrowserWindow(nullptr)
{
    _rootFolder = std::make_unique<Folder>(ASSETS_FOLDER);
}

ModuleAssets::~ModuleAssets()
{
}

bool ModuleAssets::Init()
{
    App->GetMainThreadPool()->AddTask([this]()
        {
            ScanAssetFolder();
        }
    );
    return true;
}

bool ModuleAssets::Start()
{
    _fileBrowserWindow = static_cast<FileBrowserWindow*>(App->GetModule<ModuleEditor>()->GetWindow(WindowsType::FILE_BROWSER));
    return true;
}

bool ModuleAssets::CleanUp()
{
    _guidToPath.Clear();
    _fileBrowserWindow = nullptr;
    return true;
}

void ModuleAssets::AddDroppedFiles(HDROP hDrop)
{
    auto selectedFolder = _fileBrowserWindow->GetSelectedFolder();

    char filePath[MAX_PATH];
    UINT fileCount = DragQueryFileA(hDrop, 0xFFFFFFFF, NULL, 0);

    for (UINT i = 0; i < fileCount; ++i) {
        DragQueryFileA(hDrop, i, filePath, MAX_PATH);
        std::string droppedFilePathString(filePath);
        std::replace(droppedFilePathString.begin(), droppedFilePathString.end(), '\\', '/');

        CopyAndSaveFile(droppedFilePathString);
    }
}

UID ModuleAssets::CopyAndSaveFile(const std::string& path)
{
    auto selectedFolder = _fileBrowserWindow->GetSelectedFolder();

    std::string enginePath = selectedFolder->GetPath() + '/' + ModuleFileSystem::GetFile(path.c_str());
    bool exists = ModuleFileSystem::ExistsFile(enginePath.c_str());
    ModuleFileSystem::CopyFileC(path.c_str(), enginePath.c_str());
    if (!exists)
    {
        File* file = new File(ModuleFileSystem::GetFile(path.c_str()), selectedFolder);
        CreateMetaOfFile(file);
        return file->GetMetaUID();
    }
    return selectedFolder->FindFile(enginePath)->GetMetaUID();
}

void ModuleAssets::SaveEngineFile(const std::string& path, const void* fileBuffer, size_t size, Folder* folder)
{
    bool exists = ModuleFileSystem::ExistsFile(path.c_str());
    ModuleFileSystem::SaveFile(path.c_str(), fileBuffer, size);
    if (!exists)
    {
        File* file = new File(ModuleFileSystem::GetFile(path.c_str()), folder);
        CreateMetaOfFile(file);
    }
}

UID ModuleAssets::CreateMetaFileC(const std::string& filePath)
{
    auto file = _rootFolder->FindFile(filePath);
    CreateMetaOfFile(file);
    return file->GetMetaUID();
}

void ModuleAssets::DeleteFileC(File* file)
{
    auto parentFolder = file->GetParent();
    auto metaPath = file->GetPath() + META_EXT;

    rapidjson::Document doc;
    Json json = Json(doc);
    ModuleFileSystem::LoadJson(metaPath.c_str(), json);

    UID uid = json["uid"];
    if (ModuleFileSystem::DeleteFileC(file->GetPath().c_str()) && ModuleFileSystem::DeleteFileC(metaPath.c_str()))
    {
        auto optional = _guidToPath.GetValue(uid);
        if (optional.has_value())
        {
            _guidToPath.EraseByValue(*optional);
        }
        delete parentFolder->UnlinkFile(file);
    }
    else
    {
        LOG_ERROR("Couldn't delete file or its meta: {}", file->GetPath());
    }
}

void ModuleAssets::DeleteFolder(Folder* folder)
{
    if (ModuleFileSystem::DeleteDirectory(folder->GetPath().c_str()))
    {
        // Remove all files from the GUID map
        std::queue<Folder*> foldersToCheck;
        foldersToCheck.push(folder);
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
                _guidToPath.EraseByValue(file->GetPath());
            }
        }

        auto parentFolder = folder->GetParent();
        delete parentFolder->UnlinkSubdirectory(folder);
    }
}

void ModuleAssets::ScanAssetFolder()
{
    std::vector<std::string> filesInLibPath = ModuleFileSystem::ListFilesWithPath(ASSETS_PATH);
    std::queue<std::pair<std::string, Folder*>> filesToCheck;
    for (int i = 0; i < filesInLibPath.size(); i++)
    {
        filesToCheck.emplace(filesInLibPath[i], _rootFolder.get());
    }

    File* filePreMeta = nullptr;

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
            filePreMeta = new File(ModuleFileSystem::GetFile(path.c_str()), pair.second);
            if (!ModuleFileSystem::ExistsFile((path + META_EXT).c_str()))
            {
                CreateMetaOfFile(filePreMeta);
                ProcessMetaFile(path + META_EXT, filePreMeta);
            }
        }
        else if (ModuleFileSystem::GetFileExtension(path.c_str()) == META_EXT)
        {
            if (filePreMeta && ModuleFileSystem::GetFileName(path.c_str()) == filePreMeta->GetName())
            {
                ProcessMetaFile(path, filePreMeta);
            }
        }
    }
}

void ModuleAssets::CreateMetaOfFile(File* file)
{
    std::string filePath = file->GetPath();
    rapidjson::Document doc;
    Json json = Json(doc);

    UID uid = Chiron::UIDGenerator::GenerateUID();
    file->SetMetaUID(uid);
    json["uid"] = uid;
    json["type"] = FileTypeUtils::ToString(file->GetType());
    rapidjson::StringBuffer buffer = json.ToBuffer();
    std::string metaPath = filePath + META_EXT;
    ModuleFileSystem::SaveFile(metaPath.c_str(), buffer.GetString(), (unsigned int)buffer.GetSize());

    _guidToPath.Insert(uid, filePath);
}

void ModuleAssets::ProcessMetaFile(const std::string& path, File* file)
{
    std::string expectedMetaPath = file->GetPath() + META_EXT;
    if (file == nullptr || (expectedMetaPath != path))
    {
        LOG_ERROR("Meta file in {}, doesn't have a proper file.", path);
        return;
    }
    rapidjson::Document doc;
    Json json = Json(doc);
    ModuleFileSystem::LoadJson(path.c_str(), json);

    UID uid = json["uid"];
    if (uid == 0)
    {
        LOG_WARNING("Meta file {}, missing UID.", path);
        return;
    }
    
    if (ModuleFileSystem::ExistsFile(file->GetPath().c_str()))
    {
        file->SetMetaUID(uid);
        _guidToPath.Insert(uid, file->GetPath());
    }
}
