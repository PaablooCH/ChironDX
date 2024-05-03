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

    CommandQueue* copyCQ = d3d12->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
    auto commandList = copyCQ->GetCommandList();

    ComPtr<ID3D12Resource> intermediateResource;
    d3d12->UpdateBufferResource(commandList.Get(), &_vertexBuffer, &intermediateResource, 
        vertexBufferSize / sizeof(triangleVertices[0]), vertexBufferSize, triangleVertices);

    _vertexBufferView.BufferLocation = _vertexBuffer->GetGPUVirtualAddress();
    _vertexBufferView.SizeInBytes = vertexBufferSize;
    _vertexBufferView.StrideInBytes = sizeof(Vertex);

    // -------------- INDEX ---------------------

    uint32_t indexBufferData[3] = { 0, 1, 2 };

    const UINT indexBufferSize = sizeof(indexBufferData);

    d3d12->UpdateBufferResource(commandList.Get(), &_indexBuffer, &intermediateResource,
        indexBufferSize / sizeof(indexBufferData[0]), indexBufferSize, indexBufferData);

    _indexBufferView.BufferLocation = _indexBuffer->GetGPUVirtualAddress();
    _indexBufferView.SizeInBytes = indexBufferSize;
    _indexBufferView.Format = DXGI_FORMAT_R32_UINT;

    uint64_t initFenceValue = copyCQ->ExecuteCommandList(commandList);

    copyCQ->WaitForFenceValue(initFenceValue);

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
