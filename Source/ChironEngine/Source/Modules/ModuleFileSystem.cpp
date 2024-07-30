#include "Pch.h"
#include "ModuleFileSystem.h"

#include "DataModels/FileSystem/Importers/TextureImporter.h"

#include "DataModels/DX12/Resource/Texture.h"

ModuleFileSystem::ModuleFileSystem()
{
}

ModuleFileSystem::~ModuleFileSystem()
{
}

bool ModuleFileSystem::Init()
{
    _textureImporter = std::make_unique<TextureImporter>();

	return true;
}

bool ModuleFileSystem::CleanUp()
{
	return true;
}

void ModuleFileSystem::Import(const char* filePath, std::shared_ptr<Resource> resource)
{
    _textureImporter->Import(filePath, std::dynamic_pointer_cast<Texture>(resource));
}

std::string ModuleFileSystem::GetFileExtension(const char* path)
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