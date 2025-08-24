#pragma once
#include "Asset.h"

#include "Enums/TextureType.h"

class TextureAsset;

enum MatConfig
{
    usePBR = 0x00000001,            // Use PBR or Specular as shaderType
    isOpaque = 0x00000002,          // Is opaque or translucent
};

class MaterialAsset : public Asset
{
public:
    MaterialAsset();
    MaterialAsset(UID uid);
    MaterialAsset(MaterialAsset& copy);
    ~MaterialAsset() override;

    inline bool IsValid() const override;

    // ------------- GETTERS ----------------------

    inline TextureAsset* GetTexture(TextureType textureType);
    TextureAsset* GetBaseTexture();
    TextureAsset* GetNormalMap();
    TextureAsset* GetPropertyTexture();
    TextureAsset* GetEmissiveTexture();
    TextureAsset* GetAmbientOcclusion();
    inline const Color& GetBaseColor() const;
    inline const Color& GetSpecularColor() const;
    inline UINT GetOptions() const;

    // ------------- SETTERS ----------------------

    inline void SetTexture(const std::shared_ptr<TextureAsset>& texture, TextureType textureType);
    void SetBaseTexture(const std::shared_ptr<TextureAsset>& diffuse);
    void SetNormalMap(const std::shared_ptr<TextureAsset>& normal);
    void SetPropertyTexture(const std::shared_ptr<TextureAsset>& metalness);
    void SetEmissiveTexture(const std::shared_ptr<TextureAsset>& emissive);
    void SetAmbientOcclusion(const std::shared_ptr<TextureAsset>& occlusion);
    inline void SetBaseColor(Color& color);
    inline void SetSpecularColor(Color& color);
    inline void SetOptions(UINT options);

private:
    void InternalLoad() override;
    void InternalUnload() override;

private:
    std::shared_ptr<TextureAsset> _baseTexture;
    std::shared_ptr<TextureAsset> _normalMap;
    std::shared_ptr<TextureAsset> _propertyTexture;
    std::shared_ptr<TextureAsset> _emissiveTexture;
    std::shared_ptr<TextureAsset> _ambientOcclusion;

    bool _baseTextureLoaded;
    bool _normalMapLoaded;
    bool _propertyTextureLoaded;
    bool _emissiveTextureLoaded;
    bool _ambientOcclusionLoaded;

    Color _baseColor;
    Color _specularColor;

    UINT _options;
};

inline bool MaterialAsset::IsValid() const
{
    if (_baseTexture && !_baseTextureLoaded)
    {
        return false;
    }
    if (_normalMap && !_normalMapLoaded)
    {
        return false;
    }
    if (_propertyTexture && !_propertyTextureLoaded)
    {
        return false;
    }
    if (_emissiveTexture && !_emissiveTextureLoaded)
    {
        return false;
    }
    if (_ambientOcclusion && !_ambientOcclusionLoaded)
    {
        return false;
    }
    return true;
}

inline TextureAsset* MaterialAsset::GetTexture(TextureType textureType)
{
    switch (textureType)
    {
    case TextureType::ALBEDO:
        return GetBaseTexture();
    case TextureType::METALLIC:
        return GetPropertyTexture();
    case TextureType::NORMAL_MAP:
        return GetNormalMap();
    case TextureType::EMISSIVE:
        return GetEmissiveTexture();
    case TextureType::OCCLUSION:
        return GetAmbientOcclusion();
    }
    return nullptr;
}

inline const Color& MaterialAsset::GetBaseColor() const
{
    return _baseColor;
}

inline const Color& MaterialAsset::GetSpecularColor() const
{
    return _specularColor;
}

inline UINT MaterialAsset::GetOptions() const
{
    return _options;
}

inline void MaterialAsset::SetTexture(const std::shared_ptr<TextureAsset>& texture, TextureType textureType)
{
    switch (textureType)
    {
    case TextureType::ALBEDO:
        SetBaseTexture(texture);
        break;
    case TextureType::METALLIC:
        SetPropertyTexture(texture);
        break;
    case TextureType::NORMAL_MAP:
        SetNormalMap(texture);
        break;
    case TextureType::EMISSIVE:
        SetEmissiveTexture(texture);
        break;
    case TextureType::OCCLUSION:
        SetAmbientOcclusion(texture);
        break;
    }
}

inline void MaterialAsset::SetBaseColor(Color& color)
{
    _baseColor = color;
}

inline void MaterialAsset::SetSpecularColor(Color& color)
{
    _specularColor = color;
}

inline void MaterialAsset::SetOptions(UINT options)
{
    _options = options;
}
