#pragma once

enum class TextureType
{
    ALBEDO,
    DIFFUSE = ALBEDO,
    SPECULAR,
    METALLIC = SPECULAR,
    NORMAL_MAP,
    EMISSIVE,
    OCCLUSION,
    HDR,
    DEPTH,
    RENDER_TARGET
};