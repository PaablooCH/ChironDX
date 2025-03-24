#pragma once

#include "DataModels/FileSystem/UID/UID.h"

class Folder;
class TextureAsset;

enum class FileType
{
    MATERIAL,
    MESH,
    TEXTURE,
    MODEL,
    SCENE,
    UNKNOWN
};

class File
{
public:
    File(const std::string& fileName, Folder* parent);
    ~File();

    // ------------- GETTERS ----------------------

    inline UID GetUID() const;
    inline const std::string& GetName() const;
    inline const std::string& GetDate() const;
    inline const std::string& GetSize() const;
    inline const std::string& GetPath() const;
    inline const std::string& GetExt() const;
    inline FileType GetType() const;
    inline TextureAsset* GetIcon() const;
    inline Folder* GetParent() const;

    // ------------- SETTERS ----------------------

    inline void SetParent(Folder* parent);
    inline void SetPath(const std::string& path);
    void SetParentAndMove(Folder* parent);

private:
    void CheckType();

private:
    UID _uid;
    std::string _name;
    std::string _date;
    std::string _size;
    std::string _path;
    std::string _ext;
    FileType _type;

    std::shared_ptr<TextureAsset> _icon;

    Folder* _parent;
};

inline UID File::GetUID() const
{
    return _uid;
}

inline const std::string& File::GetName() const
{
    return _name;
}

inline const std::string& File::GetDate() const
{
    return _date;
}

inline const std::string& File::GetSize() const
{
    return _size;
}

inline const std::string& File::GetPath() const
{
    return _path;
}

inline const std::string& File::GetExt() const
{
    return _ext;
}

inline FileType File::GetType() const
{
    return _type;
}

inline TextureAsset* File::GetIcon() const
{
    return _icon.get();
}

inline Folder* File::GetParent() const
{
    return _parent;
}

inline void File::SetParent(Folder* parent)
{
    _parent = parent;
}

inline void File::SetPath(const std::string& path)
{
    _path = path;
}
