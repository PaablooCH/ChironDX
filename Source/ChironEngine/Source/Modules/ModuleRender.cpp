#include "Pch.h"
#include "ModuleRender.h"

#include "Application.h"

#include "ModuleID3D12.h"
#include "ModuleProgram.h"

#include "DataModels/CommandQueue/CommandQueue.h"
#include "DataModels/Programs/Program.h"
#include "DebugDrawPass.h"

ModuleRender::ModuleRender()
{
}

ModuleRender::~ModuleRender()
{
}

bool ModuleRender::Init()
{
    auto d3d12 = App->GetModule<ModuleID3D12>();
    auto commandQueue = d3d12->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT)->GetCommandQueue();
    _debugDraw = std::make_unique<DebugDrawPass>(d3d12->GetDevice(), commandQueue);
    
    // -------------- VERTEX ---------------------
    
    // Define the geometry for a triangle.
    Vertex triangleVertices[] =
    {
        { { 0.0f, 0.25f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
        { { 0.25f, -0.25f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
        { { -0.25f, -0.25f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }
    };
    
    const UINT vertexBufferSize = sizeof(triangleVertices);

    auto device = App->GetModule<ModuleID3D12>()->GetDevice();

    // Note: using upload heaps to transfer static data like vert buffers is not 
    // recommended. Every time the GPU needs it, the upload heap will be marshalled 
    // over. Please read up on Default Heap usage. An upload heap is used here for 
    // code simplicity and because there are very few verts to actually transfer.
    CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
    auto desc = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);
    Chiron::Utils::ThrowIfFailed(device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&_vertexBuffer)));

    // Copy the triangle data to the vertex buffer.
    UINT8*  pVertexDataBegin = nullptr;
    CD3DX12_RANGE readRange(0, 0);        // We do not intend to read from this resource on the CPU.
    Chiron::Utils::ThrowIfFailed(_vertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin)));
    memcpy(pVertexDataBegin, triangleVertices, sizeof(triangleVertices));
    _vertexBuffer->Unmap(0, nullptr);

    // Initialize the vertex buffer view.
    _vertexBufferView.BufferLocation = _vertexBuffer->GetGPUVirtualAddress();
    _vertexBufferView.StrideInBytes = sizeof(Vertex);
    _vertexBufferView.SizeInBytes = vertexBufferSize;

    // -------------- INDEX ---------------------

    uint32_t indexBufferData[3] = { 0, 1, 2 };

    const UINT indexBufferSize = sizeof(indexBufferData);

    desc = CD3DX12_RESOURCE_DESC::Buffer(indexBufferSize);

    Chiron::Utils::ThrowIfFailed(device->CreateCommittedResource(
        &heapProps, 
        D3D12_HEAP_FLAG_NONE, 
        &desc,
        D3D12_RESOURCE_STATE_GENERIC_READ, 
        nullptr, 
        IID_PPV_ARGS(&_indexBuffer)));

    // Copy data to DirectX 12 driver memory:
    UINT8* pIndexDataBegin = nullptr;

    Chiron::Utils::ThrowIfFailed(_indexBuffer->Map(0, &readRange,
        reinterpret_cast<void**>(&pIndexDataBegin)));
    memcpy(pIndexDataBegin, indexBufferData, sizeof(indexBufferData));
    _indexBuffer->Unmap(0, nullptr);

    // Initialize the index buffer view.
    _indexBufferView.BufferLocation = _indexBuffer->GetGPUVirtualAddress();
    _indexBufferView.Format = DXGI_FORMAT_R32_UINT;
    _indexBufferView.SizeInBytes = indexBufferSize;

    return true;
}

UpdateStatus ModuleRender::PreUpdate()
{
    auto d3d12 = App->GetModule<ModuleID3D12>();
    
    _drawCommandList = d3d12->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT)->GetCommandList();
        
    // Transition the state to render
    d3d12->CreateTransitionBarrier(_drawCommandList, d3d12->GetRenderBuffer(), D3D12_RESOURCE_STATE_PRESENT,
        D3D12_RESOURCE_STATE_RENDER_TARGET);

    // Clear Viewport
    FLOAT clearColor[] = { 0.4f, 0.4f, 0.4f, 1.0f }; // Set color
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtv(d3d12->GetRenderTargetViewHeap()->GetCPUDescriptorHandleForHeapStart(),
        d3d12->GetCurrentBuffer(), d3d12->GetRtvSize()); // with the heap, the offset and the size, the position in memory is found

    _drawCommandList->ClearRenderTargetView(rtv, clearColor, 0, nullptr); // send the clear command into the list

    CD3DX12_CPU_DESCRIPTOR_HANDLE dsv(d3d12->GetDepthStencilViewHeap()->GetCPUDescriptorHandleForHeapStart());

    _drawCommandList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH, 1.0, 0, 0, 
        nullptr);

    return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus ModuleRender::Update()
{
    auto d3d12 = App->GetModule<ModuleID3D12>();

    d3d12->CreateTransitionBarrier(_drawCommandList, d3d12->GetRenderBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_PRESENT);

    uint64_t fenceValue = d3d12->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT)->ExecuteCommandList(_drawCommandList);
    d3d12->SaveCurrentBufferFenceValue(fenceValue);

    CHIRON_TODO("Print Grid with debugDraw");

    return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus ModuleRender::PostUpdate()
{
    App->GetModule<ModuleID3D12>()->SwapCurrentBuffer();
    return UpdateStatus::UPDATE_CONTINUE;
}

bool ModuleRender::CleanUp()
{
    return true;
}
