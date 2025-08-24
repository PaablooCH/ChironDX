#include "Pch.h"
#include "File.h"

#include "Application.h"

#include "Modules/ModuleAssets.h"
#include "Modules/ModuleFileSystem.h"
#include "Modules/ModuleResources.h"

#include "DataModels/Assets/TextureAsset.h"

#include "../Folder/Folder.h"

#include "Defines/FileSystemDefine.h"

File::File(const std::string& fileName, Folder* parent) : FileSystemEntry(fileName, parent)
{
    _parent->LinkFile(this);
    _date = ModuleFileSystem::GetModificationDateString(_path.c_str());
    _size = ModuleFileSystem::GetFileSize(_path);
    CheckType();
}

File::~File()
{
}

void File::LoadAssetIcon()
{
    if (_type == FileType::Texture && _icon == nullptr)
    {
        _icon = App->GetModule<ModuleResources>()->SearchAsset<TextureAsset>(GetUID()).get();
    }
}

TextureAsset* File::GetIcon()
{
    if (_type == FileType::Texture && _icon == nullptr)
    {
        _icon = App->GetModule<ModuleResources>()->SearchAsset<TextureAsset>(GetUID()).get();
    }
    return _icon.get();
}

void File::SetParent(Folder* parent)
{
    _parent = parent;
    std::string newPath = _parent->GetPath() + '/';
    SetPath(newPath);
}

void File::ChangeParent(Folder* parent)
{
    std::string newPath = parent->GetPath() + '/' + _name;
    if (ModuleFileSystem::MovePath(_path.c_str(), newPath.c_str()))
    {
        std::string oldMetaPath = GetPath() + META_EXT;

        std::ignore = _parent->UnlinkFile(this);
        parent->LinkFile(this);
        
        // Move the meta file to the new path
        if (ModuleFileSystem::ExistsFile(oldMetaPath.c_str()))
        {
            std::string newMetaPath = GetPath() + META_EXT;
            ModuleFileSystem::MovePath(oldMetaPath.c_str(), newMetaPath.c_str());
        }
    }
}

void File::SetPath(const std::string& path)
{
    _path = path + _name;
    App->GetModule<ModuleAssets>()->UpdateUID(GetUID(), _path);
}

void File::CheckType()
{
    std::string fileExtension = ModuleFileSystem::GetFileExtension(_name.c_str());
    
    std::transform(fileExtension.begin(), fileExtension.end(), fileExtension.begin(),
        [](unsigned char c) { return std::tolower(c); });

    _ext = fileExtension;

    if (_ext == FBX_EXT || _ext == OBJ_EXT || _ext == GLTF_EXT)
    {
        _type = FileType::Model;
    }
    else if (_ext == JPG_EXT || _ext == PNG_EXT ||
        _ext == TIF_EXT || _ext == DDS_EXT ||
        _ext == TGA_EXT || _ext == HDR_EXT)
    {
        _type = FileType::Texture;
    }
    else if (_ext == MAT_EXT)
    {
        _type = FileType::Material;
    }
    else if (_ext == MESH_EXT)
    {
        _type = FileType::Mesh;
    }
    else if (_ext == SCENE_EXT)
    {
        _type = FileType::Scene;
    }
    else
    {
        _type = FileType::UNKNOWN;
    }
}
