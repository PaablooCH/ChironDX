#include "Pch.h"
#include "FileSystemEntry.h"

#include "Modules/ModuleFileSystem.h"

#include "DataModels/FileSystem/UID/UIDGenerator.h"

FileSystemEntry::FileSystemEntry(const std::string& path) : _uid(Chiron::UIDGenerator::GenerateUID()), _name(path), 
_path(path), _parent(nullptr)
{
}

FileSystemEntry::FileSystemEntry(const std::string& path, Folder* parent) : _uid(Chiron::UIDGenerator::GenerateUID()),
_name(ModuleFileSystem::GetFile(path.c_str())), _parent(parent)
{
}

FileSystemEntry::~FileSystemEntry()
{
}
