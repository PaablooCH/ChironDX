#pragma once

#include "DataModels/FileSystem/UID/UID.h"

class Folder;

class FileSystemEntry
{
public:
    FileSystemEntry(const std::string& path);
    FileSystemEntry(const std::string& path, Folder* parent);
    virtual ~FileSystemEntry();

    // ------------- GETTERS ----------------------

    inline UID GetUID() const;
    inline const std::string& GetName() const;
    inline const std::string& GetDate() const;
    inline const std::string& GetPath() const;
    inline Folder* GetParent() const;

    // ------------- SETTERS ----------------------

    virtual void SetParent(Folder* parent) = 0;
    virtual void SetPath(const std::string& path) = 0;
    virtual void ChangeParent(Folder* parent) = 0;

protected:
    std::string _name;
    std::string _date;
    std::string _path;

    Folder* _parent;

private:
    UID _uid;
};

inline UID FileSystemEntry::GetUID() const
{
    return _uid;
}

inline const std::string& FileSystemEntry::GetName() const
{
    return _name;
}

inline const std::string& FileSystemEntry::GetDate() const
{
    return _date;
}

inline const std::string& FileSystemEntry::GetPath() const
{
    return _path;
}

inline Folder* FileSystemEntry::GetParent() const
{
    return _parent;
}
