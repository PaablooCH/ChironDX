#pragma once

#include "../FileSystemEntry.h"
#include "Enums/FileType.h"

class Folder;
class TextureAsset;

class File : public FileSystemEntry
{
public:
    File(const std::string& fileName, Folder* parent);
    ~File() override;

    // ------------- GETTERS ----------------------

    inline const std::string& GetSize() const;
    inline const std::string& GetExt() const;
    inline FileType GetType() const;
    inline UID GetMetaUID() const;
    TextureAsset* GetIcon();
    inline Folder* GetParent() const;

    // ------------- SETTERS ----------------------

    void SetParent(Folder* parent) override;
    inline void SetMetaUID (UID metaUID);
    void ChangeParent(Folder* parent) override;
    inline void SetPath(const std::string& path);

private:
    void CheckType();

private:
    std::string _size;
    std::string _ext;
    FileType _type;

    UID _metaUID;

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

inline UID File::GetMetaUID() const
{
    return _metaUID;
}

inline Folder* File::GetParent() const
{
    return _parent;
}

inline void File::SetPath(const std::string& path)
{
    _path = path;
}
