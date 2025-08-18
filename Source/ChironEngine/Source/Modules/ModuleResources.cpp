#include "Pch.h"
#include "ModuleResources.h"

#include "Application.h"

#include "ModuleAssets.h"
#include "ModuleFileSystem.h"

#include "DataModels/FileSystem/Importers/MaterialImporter.h"
#include "DataModels/FileSystem/Importers/MeshImporter.h"
#include "DataModels/FileSystem/Importers/ModelImporter.h"
#include "DataModels/FileSystem/Importers/TextureImporter.h"

#include "DataModels/Assets/MaterialAsset.h"
#include "DataModels/Assets/MeshAsset.h"
#include "DataModels/Assets/ModelAsset.h"
#include "DataModels/Assets/TextureAsset.h"

#include "Defines/FileSystemDefine.h"

ModuleResources::ModuleResources()
{
}

ModuleResources::~ModuleResources()
{
}

bool ModuleResources::Init()
{
    _textureImporter = std::make_unique<TextureImporter>();
    _materialImporter = std::make_unique<MaterialImporter>();
    _meshImporter = std::make_unique<MeshImporter>();
    _modelImporter = std::make_unique<ModelImporter>();

    _threadPool = std::make_unique<ThreadPool>(8);

    CreateLibraryFolder();
    _threadPool->AddTask([this]() 
        {
            ScanLibraryDirectory();
        }
    );
    return true;
}

bool ModuleResources::Start()
{
    return true;
}

bool ModuleResources::CleanUp()
{
    _uidToLibPath.clear();

    {
        std::unique_lock<std::mutex> lock(_mutex);
        _assets.clear();
    }
    return true;
}

void ModuleResources::ScanLibraryDirectory()
{
    std::vector<std::string> filesInLibPath = ModuleFileSystem::ListFilesWithPath(LIB_PATH);
    std::vector<std::string> allFiles;

    for (const auto& path : filesInLibPath)
    {
        if (ModuleFileSystem::IsDirectory(path.c_str()))
        {
            std::string subDir = path + "/";
            std::vector<std::string> filesInSubdir = ModuleFileSystem::ListFilesWithPath(subDir.c_str());
            allFiles.insert(allFiles.end(), filesInSubdir.begin(), filesInSubdir.end());
        }
        else
        {
            allFiles.push_back(path);
        }
    }

    for (const auto& filePath : allFiles)
    {
        UID fileName = std::stoull(ModuleFileSystem::GetFileName(filePath.c_str()));
        _uidToLibPath[fileName] = filePath;
    }
}

void ModuleResources::ProcessAsset(const std::shared_ptr<Asset>& asset, AssetOperation import)
{
    const UID uid = asset->GetUID();
    const AssetType type = asset->GetType();
    std::string path = App->GetModule<ModuleAssets>()->GetFilePath(uid);
    std::string libPath = std::to_string(uid) + BINARY_EXT;

    switch (type)
    {
    case AssetType::Material:
        if (import == AssetOperation::LOAD_LIBRARY)
        {
            path = MATERIALS_LIB_PATH + libPath;
        }
        ProcessTypedAsset<MaterialImporter, MaterialAsset>(_materialImporter.get(), path, asset, import);
        break;

    case AssetType::Mesh:
        if (import == AssetOperation::LOAD_LIBRARY)
        {
            path = MESHES_LIB_PATH + libPath;
        }
        ProcessTypedAsset<MeshImporter, MeshAsset>(_meshImporter.get(), path, asset, import);
        break;

    case AssetType::Model:
        if (import == AssetOperation::LOAD_LIBRARY)
        {
            path = MODELS_LIB_PATH + libPath;
        }
        ProcessTypedAsset<ModelImporter, ModelAsset>(_modelImporter.get(), path, asset, import);
        break;

    case AssetType::Texture:
        if (import == AssetOperation::LOAD_LIBRARY)
        {
            path = TEXTURES_LIB_PATH + libPath;
        }
        ProcessTypedAsset<TextureImporter, TextureAsset>(_textureImporter.get(), path, asset, import);
        break;

    case AssetType::UNKNOWN:
        LOG_WARNING("Try to load an UNKNOWN asset.");
        break;
    }
    if (import == AssetOperation::LOAD_LIBRARY)
    {
        _uidToLibPath[uid] = path;
    }
}

template<typename TImporter, typename TAsset>
requires ValidAssetProcessing<TImporter, TAsset>
void ModuleResources::ProcessTypedAsset(TImporter* importer, const std::string& path, const std::shared_ptr<Asset>& asset, AssetOperation op)
{
    switch (op)
    {
    case ModuleResources::AssetOperation::IMPORT:
        importer->Import(path.c_str(), std::dynamic_pointer_cast<TAsset>(asset));
        break;
    case ModuleResources::AssetOperation::LOAD_META:
        importer->LoadFromMeta(path.c_str(), std::dynamic_pointer_cast<TAsset>(asset));
        break;
    case ModuleResources::AssetOperation::LOAD_LIBRARY:
        importer->Load(path.c_str(), std::dynamic_pointer_cast<TAsset>(asset));
        break;
    }
}

std::shared_ptr<Asset> ModuleResources::LoadUID(UID uid)
{
    AssetType type;
    AssetOperation operation;
    auto it = _uidToLibPath.find(uid);
    if (it == _uidToLibPath.end()) {
        std::string metaPath = App->GetModule<ModuleAssets>()->GetFilePath(uid) + META_EXT;

        if (!ModuleFileSystem::ExistsFile(metaPath.c_str())) {
            LOG_ERROR("Meta file not found: {}", metaPath.c_str());
            return nullptr;
        }

        rapidjson::Document doc;
        Json meta = Json(doc);

        ModuleFileSystem::LoadJson(metaPath.c_str(), meta);

        std::string typeString = meta["type"];
        type = AssetTypeUtils::FromString(typeString);
        operation = AssetOperation::LOAD_META;
    }
    else
    {
        auto pathModified = ModuleFileSystem::GetPathWithoutFile(it->second);
        pathModified = ModuleFileSystem::RemoveTrailingSlash(pathModified);
        pathModified = ModuleFileSystem::GetFile(pathModified.c_str());
        type = AssetTypeUtils::GetFromFolder(pathModified);
        operation = AssetOperation::LOAD_LIBRARY;
    }
    
    auto asset = CreateAssetOfType(type, uid);
    ProcessAsset(asset, operation);
    return asset;
}

std::shared_ptr<Asset> ModuleResources::CreateNewAsset(const std::string& assetPath, AssetType type)
{
    UID uid = App->GetModule<ModuleAssets>()->CreateMetaFileC(assetPath);

    auto asset = CreateAssetOfType(type, uid);
    return asset;
}

std::shared_ptr<Asset> ModuleResources::CreateAssetOfType(AssetType type, UID uid)
{
    std::shared_ptr<Asset> asset;
    switch (type)
    {
    case AssetType::Material:
        asset = std::make_shared<MaterialAsset>(uid);
        break;
    case AssetType::Mesh:
        asset = std::make_shared<MeshAsset>(uid);
        break;
    case AssetType::Model:
        asset = std::make_shared<ModelAsset>(uid);
        break;
    case AssetType::Texture:
        asset = std::make_shared<TextureAsset>(uid);
        break;
    case AssetType::UNKNOWN:
        LOG_WARNING("Try to create an Asset with UNKNOWN type.");
        break;
    }

    if (asset)
    {
        asset->SetName(App->GetModule<ModuleAssets>()->GetFilePath(asset->GetUID()));
        
        {
            std::unique_lock<std::mutex> lock(_mutex);
            _assets[uid] = asset;
        }
    }
    return asset;
}

std::string ModuleResources::GetLibraryPath(UID uid, AssetType type)
{
    std::string path = GetLibraryPathByType(type);
    path += std::to_string(uid) + BINARY_EXT;
    return path;
}

AssetType ModuleResources::GetAssetTypeByExtension(const std::string& path)
{
    std::string fileExtension = ModuleFileSystem::GetFileExtension(path.c_str());
    std::string normalizedExtension = "";

    for (int i = 0; i < fileExtension.size(); ++i)
    {
        normalizedExtension.push_back(tolower(fileExtension[i]));
    }

    if (normalizedExtension == FBX_EXT || normalizedExtension == OBJ_EXT || normalizedExtension == GLTF_EXT)
    {
        return AssetType::Model;
    }
    else if (normalizedExtension == JPG_EXT || normalizedExtension == PNG_EXT ||
        normalizedExtension == TIF_EXT || normalizedExtension == DDS_EXT ||
        normalizedExtension == TGA_EXT || normalizedExtension == HDR_EXT)
    {
        return AssetType::Texture;
    }
    else if (normalizedExtension == MAT_EXT)
    {
        return AssetType::Material;
    }
    else if (normalizedExtension == MESH_EXT)
    {
        return AssetType::Mesh;
    }

    return AssetType::UNKNOWN;
}

std::string ModuleResources::GetLibraryPathByType(AssetType type)
{
    switch (type)
    {
    case AssetType::Material:
        return MATERIALS_LIB_PATH;
    case AssetType::Mesh:
        return MESHES_LIB_PATH;
    case AssetType::Model:
        return MODELS_LIB_PATH;
    case AssetType::Texture:
        return TEXTURES_LIB_PATH;
    case AssetType::UNKNOWN:
        LOG_WARNING("Try to get the UNKNOWN library folder.");
        break;
    };
    return "";
}

void ModuleResources::CreateLibraryFolder()
{
    if (!ModuleFileSystem::IsDirectory(LIB_FOLDER))
    {
        ModuleFileSystem::CreateDirectoryC(LIB_FOLDER);
    }

    std::vector<std::string> folders = {
        AssetTypeUtils::GetFolder(AssetType::Material), AssetTypeUtils::GetFolder(AssetType::Texture),
        AssetTypeUtils::GetFolder(AssetType::Model), AssetTypeUtils::GetFolder(AssetType::Mesh)
    };

    for (auto& folder : folders)
    {
        std::string libraryFolderOfType = LIB_PATH + folder;
        if (!ModuleFileSystem::IsDirectory(libraryFolderOfType.c_str()))
        {
            ModuleFileSystem::CreateDirectoryC(libraryFolderOfType.c_str());
        }
    }
}

UID ModuleResources::GetFileUID(const std::string& filePath) const
{
    return App->GetModule<ModuleAssets>()->GetFileUID(filePath);
}

std::shared_ptr<Asset> ModuleResources::CheckAndLoadAsset(const std::string& path, UID uid, AssetType type)
{
    std::string libraryPath = GetLibraryPath(uid, type);
    std::string metaPath = path + META_EXT;
    auto libraryDate = ModuleFileSystem::GetModificationDate(libraryPath.c_str());
    auto metaDate = ModuleFileSystem::GetModificationDate(metaPath.c_str());

    std::shared_ptr<Asset> shared = CreateAssetOfType(type, uid);
    if (metaDate <= libraryDate && metaDate != 0)
    {
        ProcessAsset(shared, AssetOperation::LOAD_LIBRARY);
    }
    else if (libraryDate == 0 && metaDate != 0)
    {
        LOG_WARNING("Library file for '{}' does not exist. Reimporting meta", path);
        ProcessAsset(shared, AssetOperation::LOAD_META);
    }
    else
    {
        LOG_WARNING("Importing Asset: {}", path);
        ProcessAsset(shared, AssetOperation::IMPORT);
    }
    return shared;
}

bool ModuleResources::ExistsFile(UID uid)
{
    return App->GetModule<ModuleAssets>()->ExistsFile(uid);
}
