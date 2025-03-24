#pragma once

#include "DataModels/FileSystem/UID/UID.h"

class File;

class Folder
{
public:
    Folder(const std::string& path);
    Folder(const std::string& path, Folder* parent);
    ~Folder();

    Folder* FindFolder(UID uid);
    Folder* FindFolder(const std::vector<std::string>& path, int iterator = 0);

    // ------------- SUBDIRECTORIES METHODS ----------------------

    void LinkSubdirectory(Folder* subdirectory);
    [[nodiscard]] Folder* UnlinkSubdirectory(Folder* subdirectory);
    bool IsSubdirectory(Folder* subdirectory);

    // ------------- FILES METHODS ----------------------

    void LinkFile(File* file);
    [[nodiscard]] File* UnlinkFile(File* file);
    bool IsFile(File* file);

    // ------------- GETTERS ----------------------

    inline UID GetUID() const;
    inline const std::string& GetName() const;
    inline const std::string& GetDate() const;
    inline const std::string& GetPath() const;
    inline bool GetOpen() const;
    inline const std::vector<std::unique_ptr<Folder>>& GetSubdirectories() const;
    inline const std::vector<std::unique_ptr<File>>& GetFiles() const;
    inline Folder* GetParent() const;
    inline bool HasSubdirectories() const;

    // ------------- SETTERS ----------------------
    
    void SetParent(Folder* parent);
    inline void SetOpened();
    inline void SetClosed();

private:
    UID _uid;
    std::string _name;
    std::string _date;
    std::string _path;
    bool _opened;

    Folder* _parent;
    std::vector<std::unique_ptr<Folder>> _subdirectories;
    std::vector<std::unique_ptr<File>> _files;
};

inline UID Folder::GetUID() const
{
    return _uid;
}

inline const std::string& Folder::GetName() const
{
    return _name;
}

inline const std::string& Folder::GetDate() const
{
    return _date;
}

inline const std::string& Folder::GetPath() const
{
    return _path;
}

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

inline Folder* Folder::GetParent() const
{
    return _parent;
}

inline bool Folder::HasSubdirectories() const
{
    return !_subdirectories.empty();
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
