#pragma once

enum class AssetType
{
    Unknown,
    Model,
    Texture,
    Mesh,
    Material
};

class Asset
{
public:

    // ------------- GETTERS ----------------------

    inline virtual std::wstring GetName() const = 0;
    inline virtual AssetType GetType() const;

protected:
    Asset(AssetType type);
    virtual ~Asset();

private:
    Asset();

private:
    AssetType _type;
};

inline AssetType Asset::GetType() const
{
    return _type;
}