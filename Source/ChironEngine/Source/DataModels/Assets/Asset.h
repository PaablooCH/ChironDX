#pragma once

#include "Enums/AssetType.h"
#include "DataModels/FileSystem/UID/UID.h"
#include "Defines/FileSystemDefine.h"

class Asset
{
public:
    inline bool IsValid() const;

    bool Load();
    bool Unload();

    // ------------- GETTERS ----------------------

    inline const UID GetUID() const;
    inline virtual const std::string& GetName() const;
    inline AssetType GetType() const;

    // ------------- SETTERS ----------------------

    inline void SetName(const std::string& name);

protected:
    Asset(UID uid, AssetType type);
    Asset(AssetType type);
    virtual ~Asset();

    virtual bool InternalLoad() { return true; };
    virtual bool InternalUnload() { return false; };

private:
    Asset();

private:
    UID _uid;
    std::string _name;
    AssetType _type;

    bool _loaded;
};

inline bool Asset::IsValid() const
{
    return _loaded;
}

inline const UID Asset::GetUID() const
{
    return _uid;
}

inline const std::string& Asset::GetName() const
{
    return _name;
}

inline AssetType Asset::GetType() const
{
    return _type;
}

inline void Asset::SetName(const std::string& name)
{
    _name = name;
}
