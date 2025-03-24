#pragma once

#include "../FileSystemEntry.h"

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

class File : public FileSystemEntry
{
public:
    File(const std::string& fileName, Folder* parent);
    ~File() override;

    // ------------- GETTERS ----------------------

    inline const std::string& GetSize() const;
    inline const std::string& GetExt() const;
    inline FileType GetType() const;
    inline TextureAsset* GetIcon() const;
    inline Folder* GetParent() const;

    // ------------- SETTERS ----------------------

    inline void SetParent(Folder* parent) override;
    void ChangeParent(Folder* parent) override;
    inline void SetPath(const std::string& path);

private:
    void CheckType();

private:
    std::string _size;
    std::string _ext;
    FileType _type;

    std::shared_ptr<TextureAsset> _icon;
};

inline const std::string& File::GetSize() const
{
    return _size;
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
