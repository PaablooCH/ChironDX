#pragma once
#include "Asset.h"

class IndexBuffer;
class VertexBuffer;

struct Vertex
{
    Vector3 vertices;
    Vector2 texCoords;
    Vector3 normals;
    Vector3 tangents;
    Vector3 biTangents;
};

class MeshAsset : public Asset
{
public:
    MeshAsset();
    ~MeshAsset() override;

    // ------------- GETTERS ----------------------

    inline std::wstring GetName() const override;
    inline IndexBuffer* GetIndexBuffer() const;
    inline VertexBuffer* GetVertexBuffer() const;

    // ------------- GETTERS ----------------------

    void SetIndexBuffer(const D3D12_RESOURCE_DESC& resourceDesc, size_t numIndices, const DXGI_FORMAT& indexFormat,
        const std::wstring& name = L"");
    void SetVertexBuffer(const D3D12_RESOURCE_DESC& resourceDesc, size_t numVertices, const std::wstring& name = L"");

private:
    std::unique_ptr<IndexBuffer> _indexBuffer;
    std::unique_ptr<VertexBuffer> _vertexBuffer;
};

inline std::wstring MeshAsset::GetName() const
{
    CHIRON_TODO("TODO");
    return L"";
}

inline IndexBuffer* MeshAsset::GetIndexBuffer() const
{
    return _indexBuffer.get();
}

inline VertexBuffer* MeshAsset::GetVertexBuffer() const
{
    return _vertexBuffer.get();
}
