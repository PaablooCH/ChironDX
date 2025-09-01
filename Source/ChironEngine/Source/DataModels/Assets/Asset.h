#pragma once

#include "DataModels/FileSystem/UID/UID.h"
#include "Enums/AssetType.h"

class Asset
{
public:
    bool Load();
    bool Unload();

    virtual bool IsValid() const = 0;

    // ------------- GETTERS ----------------------

    inline const UID GetUID() const;
    inline virtual const std::string& GetName() const;
    inline AssetType GetType() const;

    // ------------- SETTERS ----------------------

    inline void SetName(const std::string& name);

protected:
    Asset(UID uid, AssetType type);
    Asset(AssetType type);
    Asset(Asset& copy);
    virtual ~Asset();

    virtual void InternalLoad() {};
    virtual void InternalUnload() {};

private:
    Asset();

private:
    UID _uid;
    std::string _name;
    AssetType _type;
};

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
