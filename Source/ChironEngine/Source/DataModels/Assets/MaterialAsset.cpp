#include "Pch.h"
#include "MaterialAsset.h"

#include "TextureAsset.h"

MaterialAsset::MaterialAsset() : Asset(AssetType::Material), _baseColor(1.f, 1.f, 1.f, 1.f),
_specularColor(0.5f, 0.30000001192092896f, 0.5f, 1.f), _options(usePBR | isOpaque), _baseTextureLoaded(false), _normalMapLoaded(false),
_propertyTextureLoaded(false), _emissiveTextureLoaded(false), _ambientOcclusionLoaded(false)
{
}

MaterialAsset::MaterialAsset(UID uid) : Asset(uid, AssetType::Material), _baseColor(1.f, 1.f, 1.f, 1.f),
_specularColor(0.5f, 0.30000001192092896f, 0.5f, 1.f), _options(usePBR | isOpaque), _baseTextureLoaded(false), _normalMapLoaded(false),
_propertyTextureLoaded(false), _emissiveTextureLoaded(false), _ambientOcclusionLoaded(false)
{
}

MaterialAsset::MaterialAsset(MaterialAsset& copy) : Asset(copy), _baseTexture(copy._baseTexture), _normalMap(copy._normalMap), 
_propertyTexture(copy._propertyTexture), _emissiveTexture(copy._emissiveTexture), _ambientOcclusion(copy._ambientOcclusion), 
_baseColor(copy._baseColor), _specularColor(copy._specularColor), _options(copy._options), _baseTextureLoaded(false), _normalMapLoaded(false),
_propertyTextureLoaded(false), _emissiveTextureLoaded(false), _ambientOcclusionLoaded(false)
{
}

MaterialAsset::~MaterialAsset()
{
}

TextureAsset* MaterialAsset::GetBaseTexture()
{
    if (_baseTexture)
    {
        _baseTexture->Load();
    }
    return _baseTexture.get();
}

TextureAsset* MaterialAsset::GetNormalMap()
{
    if (_normalMap)
    {
        _normalMap->Load();
    }
    return _normalMap.get();
}

TextureAsset* MaterialAsset::GetPropertyTexture()
{
    if (_propertyTexture)
    {
        _propertyTexture->Load();
    }
    return _propertyTexture.get();
}

TextureAsset* MaterialAsset::GetEmissiveTexture()
{
    if (_emissiveTexture)
    {
        _emissiveTexture->Load();
    }
    return _emissiveTexture.get();
}

TextureAsset* MaterialAsset::GetAmbientOcclusion()
{
    if (_ambientOcclusion)
    {
        _ambientOcclusion->Load();
    }
    return _ambientOcclusion.get();
}

void MaterialAsset::SetBaseTexture(const std::shared_ptr<TextureAsset>& diffuse)
{
    _baseTexture = diffuse;
    if (_baseTexture)
    {
        _baseTexture->SetTextureType(TextureType::ALBEDO);
    }
}

void MaterialAsset::SetNormalMap(const std::shared_ptr<TextureAsset>& normal)
{
    _normalMap = normal;
    if (_normalMap)
    {
        _normalMap->SetTextureType(TextureType::NORMAL_MAP);
    }
}

void MaterialAsset::SetPropertyTexture(const std::shared_ptr<TextureAsset>& metalness)
{
    _propertyTexture = metalness;
    if (_propertyTexture)
    {
        _propertyTexture->SetTextureType(TextureType::METALLIC);
    }
}

void MaterialAsset::SetEmissiveTexture(const std::shared_ptr<TextureAsset>& emissive)
{
    _emissiveTexture = emissive;
    if (_emissiveTexture)
    {
        _emissiveTexture->SetTextureType(TextureType::EMISSIVE);
    }
}

void MaterialAsset::SetAmbientOcclusion(const std::shared_ptr<TextureAsset>& occlusion)
{
   _ambientOcclusion = occlusion;
    if (_ambientOcclusion)
    {
        _ambientOcclusion->SetTextureType(TextureType::OCCLUSION);
    }
}

void MaterialAsset::InternalLoad()
{
    if (_baseTexture && !_baseTextureLoaded)
    {
        _baseTextureLoaded = _baseTexture->Load();
    }
    if (_normalMap && !_normalMapLoaded)
    {
        _normalMapLoaded = _normalMap->Load();
    }
    if (_propertyTexture && !_propertyTextureLoaded)
    {
        _propertyTextureLoaded = _propertyTexture->Load();
    }
    if (_emissiveTexture && !_emissiveTextureLoaded)
    {
        _emissiveTextureLoaded = _emissiveTexture->Load();
    }
    if (_ambientOcclusion && !_ambientOcclusionLoaded)
    {
        _ambientOcclusionLoaded = _ambientOcclusion->Load();
    }
}

void MaterialAsset::InternalUnload()
{
    if (_baseTexture && _baseTextureLoaded)
    {
        _baseTextureLoaded = !_baseTexture->Unload();
    }
    if (_normalMap && _normalMapLoaded)
    {
        _normalMapLoaded = !_normalMap->Unload();
    }
    if (_propertyTexture && _propertyTextureLoaded)
    {
        _propertyTextureLoaded = !_propertyTexture->Unload();
    }
    if (_emissiveTexture && _emissiveTextureLoaded)
    {
        _emissiveTextureLoaded = !_emissiveTexture->Unload();
    }
    if (_ambientOcclusion && _ambientOcclusionLoaded)
    {
        _ambientOcclusionLoaded = !_ambientOcclusion->Unload();
    }
}