#pragma once

#include <d3d12.h>
#include <functional>

namespace std {
    template <>
    struct hash<D3D12_SHADER_RESOURCE_VIEW_DESC> {
        std::size_t operator()(const D3D12_SHADER_RESOURCE_VIEW_DESC& desc) const noexcept {
            std::size_t hashValue = 0;

            // Hash each member of the D3D12_SHADER_RESOURCE_VIEW_DESC struct
            hashValue ^= std::hash<UINT>{}(desc.Format) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
            hashValue ^= std::hash<D3D12_SRV_DIMENSION>{}(desc.ViewDimension) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
            hashValue ^= std::hash<UINT>{}(desc.Shader4ComponentMapping) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);

            // Depending on the ViewDimension, hash the appropriate union member
            switch (desc.ViewDimension) {
            case D3D12_SRV_DIMENSION_BUFFER:
                hashValue ^= std::hash<UINT64>{}(desc.Buffer.FirstElement) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
                hashValue ^= std::hash<UINT>{}(desc.Buffer.NumElements) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
                hashValue ^= std::hash<UINT>{}(desc.Buffer.StructureByteStride) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
                hashValue ^= std::hash<D3D12_BUFFER_SRV_FLAGS>{}(desc.Buffer.Flags) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
                break;
            case D3D12_SRV_DIMENSION_TEXTURE1D:
                hashValue ^= std::hash<UINT>{}(desc.Texture1D.MostDetailedMip) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
                hashValue ^= std::hash<UINT>{}(desc.Texture1D.MipLevels) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
                hashValue ^= std::hash<FLOAT>{}(desc.Texture1D.ResourceMinLODClamp) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
                break;
            case D3D12_SRV_DIMENSION_TEXTURE1DARRAY:
                hashValue ^= std::hash<UINT>{}(desc.Texture1DArray.MostDetailedMip) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
                hashValue ^= std::hash<UINT>{}(desc.Texture1DArray.MipLevels) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
                hashValue ^= std::hash<UINT>{}(desc.Texture1DArray.FirstArraySlice) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
                hashValue ^= std::hash<UINT>{}(desc.Texture1DArray.ArraySize) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
                hashValue ^= std::hash<FLOAT>{}(desc.Texture1DArray.ResourceMinLODClamp) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
                break;
            case D3D12_SRV_DIMENSION_TEXTURE2D:
                hashValue ^= std::hash<UINT>{}(desc.Texture2D.MostDetailedMip) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
                hashValue ^= std::hash<UINT>{}(desc.Texture2D.MipLevels) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
                hashValue ^= std::hash<UINT>{}(desc.Texture2D.PlaneSlice) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
                hashValue ^= std::hash<FLOAT>{}(desc.Texture2D.ResourceMinLODClamp) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
                break;
            case D3D12_SRV_DIMENSION_TEXTURE2DARRAY:
                hashValue ^= std::hash<UINT>{}(desc.Texture2DArray.MostDetailedMip) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
                hashValue ^= std::hash<UINT>{}(desc.Texture2DArray.MipLevels) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
                hashValue ^= std::hash<UINT>{}(desc.Texture2DArray.FirstArraySlice) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
                hashValue ^= std::hash<UINT>{}(desc.Texture2DArray.ArraySize) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
                hashValue ^= std::hash<UINT>{}(desc.Texture2DArray.PlaneSlice) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
                hashValue ^= std::hash<FLOAT>{}(desc.Texture2DArray.ResourceMinLODClamp) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
                break;
            case D3D12_SRV_DIMENSION_TEXTURE3D:
                hashValue ^= std::hash<UINT>{}(desc.Texture3D.MostDetailedMip) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
                hashValue ^= std::hash<UINT>{}(desc.Texture3D.MipLevels) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
                hashValue ^= std::hash<FLOAT>{}(desc.Texture3D.ResourceMinLODClamp) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
                break;
            case D3D12_SRV_DIMENSION_TEXTURECUBE:
                hashValue ^= std::hash<UINT>{}(desc.TextureCube.MostDetailedMip) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
                hashValue ^= std::hash<UINT>{}(desc.TextureCube.MipLevels) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
                hashValue ^= std::hash<FLOAT>{}(desc.TextureCube.ResourceMinLODClamp) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
                break;
            case D3D12_SRV_DIMENSION_TEXTURECUBEARRAY:
                hashValue ^= std::hash<UINT>{}(desc.TextureCubeArray.MostDetailedMip) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
                hashValue ^= std::hash<UINT>{}(desc.TextureCubeArray.MipLevels) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
                hashValue ^= std::hash<UINT>{}(desc.TextureCubeArray.First2DArrayFace) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
                hashValue ^= std::hash<UINT>{}(desc.TextureCubeArray.NumCubes) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
                hashValue ^= std::hash<FLOAT>{}(desc.TextureCubeArray.ResourceMinLODClamp) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
                break;
            default:
                break;
            }

            return hashValue;
        }
    };
}

