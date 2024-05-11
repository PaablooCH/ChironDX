#pragma once
#include "Module.h"

class DebugDrawPass;

struct Vertex
{
    Vector3 position;
    Vector4 color;
};

class ModuleRender : public Module
{
public:
    ModuleRender();
    ~ModuleRender();

    bool Init() override;
    UpdateStatus PreUpdate() override;
    UpdateStatus Update() override;
    UpdateStatus PostUpdate() override;
    bool CleanUp() override;

private:
    
private:
    // Indicate to the driver how a resource should be used in upcoming commands.
    // Is used once its loaded into a Queue
    ComPtr<ID3D12GraphicsCommandList> _drawCommandList;

    ComPtr<ID3D12Resource> _vertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW _vertexBufferView;

    ComPtr<ID3D12Resource> _indexBuffer;
    D3D12_INDEX_BUFFER_VIEW _indexBufferView;

    std::unique_ptr<DebugDrawPass> _debugDraw;

    D3D12_RECT _scissor;
};