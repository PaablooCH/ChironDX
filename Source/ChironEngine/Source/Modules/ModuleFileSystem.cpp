#include "Pch.h"
#include "ModuleFileSystem.h"

#include "DataModels/FileSystem/Importers/ModelImporter.h"
#include "DataModels/FileSystem/Importers/TextureImporter.h"

#include "DataModels/Assets/ModelAsset.h"
#include "DataModels/Assets/TextureAsset.h"

ModuleFileSystem::ModuleFileSystem()
{
}

ModuleFileSystem::~ModuleFileSystem()
{
}

bool ModuleFileSystem::Init()
{
    _textureImporter = std::make_unique<TextureImporter>();
    _modelImporter = std::make_unique<ModelImporter>();

	return true;
}

bool ModuleFileSystem::CleanUp()
{
	return true;
}

void ModuleFileSystem::Import(const char* filePath, const std::shared_ptr<Asset>& asset)
{
    switch (asset->GetType())
    {
    case AssetType::Model:
        _modelImporter->Import(filePath, std::dynamic_pointer_cast<ModelAsset>(asset));
        break;
    case AssetType::Texture:
        _textureImporter->Import(filePath, std::dynamic_pointer_cast<TextureAsset>(asset));
        break;
    case AssetType::Mesh:
        break;
    default:
        break;
    }
}

const std::string ModuleFileSystem::GetFileExtension(const char* path)
{
    std::string sPath(path);
    size_t dotPosition = sPath.find_last_of('.');
    size_t slashPosition = sPath.find_last_of("/");
    if (dotPosition != std::string::npos && (slashPosition == std::string::npos || dotPosition > slashPosition)) {
        return sPath.substr(dotPosition);
    }
    else {
        return ""; // No extension found or dot is in a directory name
    }
}

const std::string ModuleFileSystem::GetFileName(const std::string& path)
{
    std::string result = "";

    size_t lastSlash = path.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        result = path.substr(lastSlash + 1);
    }
    else {
        result = path;
    }

    size_t lastDot = result.find_last_of(".");
    if (lastDot != std::string::npos) {
        result = result.substr(0, lastDot);
    }
    return result;
}
