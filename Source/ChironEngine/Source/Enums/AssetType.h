#pragma once
#include "EnumNames.h"

DECLARE_ENUM_NAMES(AssetType, Material,Mesh,Model,Texture)

namespace AssetTypeUtils
{
    inline std::string GetFolder(AssetType type)
    {
        switch (type)
        {
        case AssetType::Material:
            return "Materials";
        case AssetType::Mesh:
            return "Meshes";
        case AssetType::Model:
            return "Models";
        case AssetType::Texture:
            return "Textures";
        case AssetType::UNKNOWN:
            LOG_ERROR("Try to get folder UKNOWN");
            return "Unknown";
            break;
        default:
            LOG_ERROR("Unhandled AssetType in GetFolder.");
            return "Invalid";
        }
    }

    inline AssetType GetFromFolder(const std::string& folderName)
    {
        if (folderName == "Materials")
        {
            return AssetType::Material;
        }
        if (folderName == "Meshes")
        {
            return AssetType::Mesh;
        }
        if (folderName == "Models")
        {
            return AssetType::Model;
        }
        if (folderName == "Textures")
        {
            return AssetType::Texture;
        }
        return AssetType::UNKNOWN;
    }
}