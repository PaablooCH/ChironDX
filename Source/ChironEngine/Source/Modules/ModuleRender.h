#pragma once
#include "Module.h"

class CommandList;
class IndexBuffer;
class Texture;
class VertexBuffer;
class DebugDrawPass;

struct Vertex
{
    Vector3 position;
    Vector2 texCoord;
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
    struct ModelViewProjection
    {
        Matrix model;
        Matrix view;
        Matrix proj;
    };
    // Indicate to the driver how a resource should be used in upcoming commands.
    // Is used once its loaded into a Queue
    std::shared_ptr<CommandList> _drawCommandList;

    std::unique_ptr<DebugDrawPass> _debugDraw;
    std::shared_ptr<Texture> texture;
    std::shared_ptr<VertexBuffer> vertexBuffer;
    std::shared_ptr<IndexBuffer> indexBuffer;

    D3D12_RECT _scissor;
};