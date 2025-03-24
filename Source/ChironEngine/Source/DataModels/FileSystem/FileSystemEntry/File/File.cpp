#include "Pch.h"
#include "File.h"

#include "Application.h"

#include "Modules/ModuleFileSystem.h"
#include "Modules/ModuleResources.h"

#include "DataModels/Assets/TextureAsset.h"

#include "../Folder/Folder.h"

#include "DataModels/FileSystem/UID/UIDGenerator.h"
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

void File::ChangeParent(Folder* parent)
{
    std::string newPath = parent->GetPath() + '/' + _name;
    if (ModuleFileSystem::MovePath(_path.c_str(), newPath.c_str()))
    {
        std::ignore = _parent->UnlinkFile(this);
        parent->LinkFile(this);
    }
}

void File::CheckType()
{
    std::string fileExtension = ModuleFileSystem::GetFileExtension(_name.c_str());
    auto moduleResources = App->GetModule<ModuleResources>();

    std::transform(fileExtension.begin(), fileExtension.end(), fileExtension.begin(),
        [](unsigned char c) { return std::tolower(c); });

    _ext = fileExtension;

    if (_ext == FBX_EXT || _ext == OBJ_EXT || _ext == GLTF_EXT)
    {
        _type = FileType::MODEL;
    }
    else if (_ext == JPG_EXT || _ext == PNG_EXT ||
        _ext == TIF_EXT || _ext == DDS_EXT ||
        _ext == TGA_EXT || _ext == HDR_EXT)
    {
        _type = FileType::TEXTURE;
        _icon = moduleResources->RequestAsset<TextureAsset>(_path).get();
    }
    else if (_ext == MAT_EXT)
    {
        _type = FileType::MATERIAL;
    }
    else if (_ext == MESH_EXT)
    {
        _type = FileType::MESH;
    }
    else if (_ext == SCENE_EXT)
    {
        _type = FileType::SCENE;
    }
    else
    {
        _type = FileType::UNKNOWN;
    }
}
