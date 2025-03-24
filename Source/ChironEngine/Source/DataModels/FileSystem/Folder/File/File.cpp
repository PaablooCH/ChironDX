#include "Pch.h"
#include "File.h"

#include "Application.h"

#include "Modules/ModuleFileSystem.h"
#include "Modules/ModuleResources.h"

#include "DataModels/Assets/TextureAsset.h"

#include "DataModels/FileSystem/Folder/Folder.h"

#include "DataModels/FileSystem/UID/UIDGenerator.h"
#include "Defines/FileSystemDefine.h"

File::File(const std::string& fileName, Folder* parent) : _uid(Chiron::UIDGenerator::GenerateUID()),
_name(fileName), _parent(parent)
{
}

File::~File()
{
}

void File::SetParentAndMove(Folder* parent)
{
}

void File::CheckType()
{
}
