#pragma once
#include "Asset.h"

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

    TextureAsset* GetBaseTexture();
    TextureAsset* GetNormalMap();
    TextureAsset* GetPropertyTexture();
    TextureAsset* GetEmissiveTexture();
    TextureAsset* GetAmbientOcclusion();
    inline const Color& GetBaseColor() const;
    inline const Color& GetSpecularColor() const;
    inline UINT GetOptions() const;

    // ------------- SETTERS ----------------------

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
