#include "Pch.h"
#include "Asset.h"

#include "DataModels/FileSystem/UID/UIDGenerator.h"

Asset::Asset(UID uid, AssetType type) : _uid(uid), _type(type)
{
}

Asset::Asset(AssetType type) : _uid(Chiron::UIDGenerator::GenerateUID()), _type(type)
{
}

Asset::Asset(Asset& copy) : _uid(Chiron::UIDGenerator::GenerateUID()), _name(copy._name), _type(copy._type)
{
}

Asset::~Asset()
{
}

bool Asset::Load()
{
    if (!IsValid())
    {
        InternalLoad();
    }
    return IsValid();
}

bool Asset::Unload()
{
    if (IsValid())
    {
        InternalUnload();
    }
    return !IsValid();
}