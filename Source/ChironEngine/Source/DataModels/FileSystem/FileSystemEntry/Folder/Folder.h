#pragma once

#include "../FileSystemEntry.h"

class File;

class Folder : public FileSystemEntry
{
public:
    Folder(const std::string& path);
    Folder(const std::string& path, Folder* parent);
    ~Folder() override;

    FileSystemEntry* FindFileSystemEntry(UID uid);
    Folder* FindFolder(const std::vector<std::string>& path, int iterator = 0);
    File* FindFile(const std::string& path);

    // ------------- SUBDIRECTORIES METHODS ----------------------

    void LinkSubdirectory(Folder* subdirectory);
    [[nodiscard]] Folder* UnlinkSubdirectory(Folder* subdirectory);
    bool IsSubdirectory(Folder* subdirectory);

    // ------------- FILES METHODS ----------------------

    void LinkFile(File* file);
    [[nodiscard]] File* UnlinkFile(File* file);
    bool IsFile(File* file);

    // ------------- GETTERS ----------------------

    inline bool GetOpen() const;
    inline const std::vector<std::unique_ptr<Folder>>& GetSubdirectories() const;
    inline const std::vector<std::unique_ptr<File>>& GetFiles() const;
    inline bool HasSubdirectories() const;

    // ------------- SETTERS ----------------------
    
    inline void SetParent(Folder* parent) override;
    void ChangeParent(Folder* parent) override;
    inline void SetOpened();
    inline void SetClosed();

private:
    bool _opened;

    std::vector<std::unique_ptr<Folder>> _subdirectories;
    std::vector<std::unique_ptr<File>> _files;
};

inline bool Folder::GetOpen() const
{
    return _opened;
}

inline const std::vector<std::unique_ptr<Folder>>& Folder::GetSubdirectories() const
{
    return _subdirectories;
}

inline const std::vector<std::unique_ptr<File>>& Folder::GetFiles() const
{
    return _files;
}

inline bool Folder::HasSubdirectories() const
{
    return !_subdirectories.empty();
}

inline void Folder::SetParent(Folder* parent)
{
    _parent = parent;
    std::string newPath = parent->_path + '/';
    SetPath(newPath);
}

inline void Folder::SetOpened()
{
    _opened = true;
    if (_parent)
    {
        _parent->SetOpened();
    }
}

inline void Folder::SetClosed()
{
    _opened = false;
}
