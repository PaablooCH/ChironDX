#include "Pch.h"
#include "Folder.h"

#include "Modules/ModuleFileSystem.h"

#include "../File/File.h"


Folder::Folder(const std::string& path) : FileSystemEntry(path), _opened(false)
{
}

Folder::Folder(const std::string& path, Folder* parent) : FileSystemEntry(path, parent), _opened(false)
{
    _parent->LinkSubdirectory(this);
    _date = ModuleFileSystem::GetModificationDateString(_path.c_str());
}

Folder::~Folder()
{
}

FileSystemEntry* Folder::FindFolder(UID uid)
{
    std::queue<Folder*> queue;
    queue.push(this);
    while (!queue.empty())
    {
        Folder* folder = queue.front();
        queue.pop();
        if (folder->GetUID() == uid)
        {
            return folder;
        }

        for (auto& file : folder->GetFiles())
        {
            if (file->GetUID() == uid)
            {
                return file.get();
            }
        }

        for (auto& subdirectory : folder->GetSubdirectories())
        {
            queue.push(subdirectory.get());
        }
    }
    return nullptr;
}

Folder* Folder::FindFolder(const std::vector<std::string>& path, int iterator /* = 0 */)
{
    if (iterator == path.size() - 1)
    {
        return this;
    }
    iterator++;
    for (auto& sub : _subdirectories)
    {
        if (sub->GetName() == path[iterator])
        {
            return sub->FindFolder(path, iterator);
        }
    }
    LOG_ERROR("Didn't find folder");
    return nullptr;
}

void Folder::LinkSubdirectory(Folder* subdirectory)
{
    assert(subdirectory);

    if (!IsSubdirectory(subdirectory))
    {
        subdirectory->_parent = this;
        std::string newPath = _path + '/' + subdirectory->_name;
        subdirectory->_path = newPath;
        _subdirectories.push_back(std::unique_ptr<Folder>(subdirectory));
    }
}

Folder* Folder::UnlinkSubdirectory(Folder* subdirectory)
{
    assert(subdirectory);
    if (IsSubdirectory(subdirectory))
    {
        auto childIt = std::ranges::find_if(_subdirectories,
            [subdirectory](std::unique_ptr<Folder>& actualChild)
            {
                return actualChild.get() == subdirectory;
            });

        auto orphan = childIt->release();
        orphan->_parent = nullptr;
        _subdirectories.erase(childIt);

        return orphan;
    }
    return nullptr;
}

bool Folder::IsSubdirectory(Folder* subdirectory)
{
    return std::ranges::any_of(_subdirectories.begin(), _subdirectories.end(),
        [subdirectory](std::unique_ptr<Folder>& actualChild)
        {
            return actualChild.get() == subdirectory;
        });
}

void Folder::LinkFile(File* file)
{
    assert(file);

    if (!IsFile(file))
    {
        file->SetParent(this);
        std::string newPath = _path + '/' + file->GetName();
        file->SetPath(newPath);
        _files.push_back(std::unique_ptr<File>(file));
    }
}

File* Folder::UnlinkFile(File* file)
{
    assert(file);
    if (IsFile(file))
    {
        auto childIt = std::ranges::find_if(_files,
            [file](std::unique_ptr<File>& actualChild)
            {
                return actualChild.get() == file;
            });

        auto orphan = childIt->release();
        _files.erase(childIt);

        return orphan;
    }
    return nullptr;
}

bool Folder::IsFile(File* file)
{
    return std::ranges::any_of(_files.begin(), _files.end(),
        [file](std::unique_ptr<File>& actualChild)
        {
            return actualChild.get() == file;
        });
}

void Folder::ChangeParent(Folder* parent)
{
    if (parent->IsSubdirectory(this))
    {
        return;
    }
    std::string newPath = parent->_path + '/' + _name;
    if (ModuleFileSystem::MovePath(_path.c_str(), newPath.c_str()))
    {
        std::ignore = _parent->UnlinkSubdirectory(this);
        parent->LinkSubdirectory(this);
    }
}
