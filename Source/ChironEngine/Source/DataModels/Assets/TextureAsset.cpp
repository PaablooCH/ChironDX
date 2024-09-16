#include "Pch.h"
#include "TextureAsset.h"

#include "DataModels/DX12/Resource/Texture.h"

TextureAsset::TextureAsset(TextureType type) : Asset(AssetType::Texture), _type(type)
{
}

TextureAsset::~TextureAsset()
{
}

std::wstring TextureAsset::GetName() const
{
    return _texture->GetName();
}