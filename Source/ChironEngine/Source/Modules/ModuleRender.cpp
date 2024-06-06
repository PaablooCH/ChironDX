#include "Pch.h"
#include "ModuleRender.h"

#include "Application.h"

#include "ModuleCamera.h"
#include "ModuleID3D12.h"
#include "ModuleProgram.h"
#include "ModuleWindow.h"

#include "DataModels/DX12/CommandQueue/CommandQueue.h"
#include "DataModels/Programs/Program.h"

#include "DebugDrawPass.h"

ModuleRender::ModuleRender() : _scissor(CD3DX12_RECT(0, 0, LONG_MAX, LONG_MAX))
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
    
    CommandQueue* copyCQ = d3d12->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
    auto commandList = copyCQ->GetCommandList();
    
    // Define the geometry for a triangle.
    Vertex triangleVertices[] =
    {
        { { 0.0f, 0.25f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
        { { 0.25f, -0.25f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
        { { -0.25f, -0.25f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }
    };
    
    const UINT vertexBufferSize = sizeof(triangleVertices);

    ComPtr<ID3D12Resource> intermediateResource;
    d3d12->UpdateBufferResource(commandList.Get(), &_vertexBuffer, &intermediateResource, 
        vertexBufferSize / sizeof(triangleVertices[0]), vertexBufferSize, triangleVertices);

    _vertexBuffer->SetName(L"Triangle Vertex Buffer");

    // Tell the input assembler where the vertices are
    _vertexBufferView.BufferLocation = _vertexBuffer->GetGPUVirtualAddress();
    _vertexBufferView.SizeInBytes = vertexBufferSize;
    _vertexBufferView.StrideInBytes = sizeof(Vertex);

    // -------------- INDEX ---------------------

    uint32_t indexBufferData[3] = { 0, 1, 2 };

    const UINT indexBufferSize = sizeof(indexBufferData);

    ComPtr<ID3D12Resource> intermediateResource2;
    d3d12->UpdateBufferResource(commandList.Get(), &_indexBuffer, &intermediateResource2,
        indexBufferSize / sizeof(indexBufferData[0]), indexBufferSize, indexBufferData);

    _indexBuffer->SetName(L"Triangle Index Buffer");

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

    _drawCommandList->ClearRenderTargetView(d3d12->GetRenderTargetDescriptor(), clearColor, 0, nullptr); // send the clear command into the list

    _drawCommandList->ClearDepthStencilView(d3d12->GetDepthStencilDescriptor(), D3D12_CLEAR_FLAG_DEPTH, 1.0, 0, 0, 
        nullptr);

    return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus ModuleRender::Update()
{
    auto d3d12 = App->GetModule<ModuleID3D12>();
    auto programs = App->GetModule<ModuleProgram>();
    auto window = App->GetModule<ModuleWindow>();
    auto camera = App->GetModule<ModuleCamera>();

    Program* defaultP = programs->GetProgram(ProgramType::DEFAULT);

    _drawCommandList->SetPipelineState(defaultP->GetPipelineState());
    _drawCommandList->SetGraphicsRootSignature(defaultP->GetRootSignature());

    _drawCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    _drawCommandList->IASetVertexBuffers(0, 1, &_vertexBufferView);
    _drawCommandList->IASetIndexBuffer(&_indexBufferView);

    unsigned width;
    unsigned height;
    window->GetWindowSize(width, height);

    D3D12_VIEWPORT viewport{};
    viewport.TopLeftX = viewport.TopLeftY = 0;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.Width = static_cast<float>(width);
    viewport.Height = static_cast<float>(height);

    _drawCommandList->RSSetViewports(1, &viewport);
    _drawCommandList->RSSetScissorRects(1, &_scissor);

    auto rtv = d3d12->GetRenderTargetDescriptor();
    auto dsv = d3d12->GetDepthStencilDescriptor();
    _drawCommandList->OMSetRenderTargets(1, &rtv, FALSE, &dsv);

    Matrix model = Matrix::Identity;
    Matrix view = camera->GetViewMatrix();
    Matrix proj = camera->GetProjMatrix();

    Matrix mvp = model * view;
    mvp = mvp * proj;
    _drawCommandList->SetGraphicsRoot32BitConstants(0, sizeof(mvp) / 4, &mvp, 0);

    _drawCommandList->DrawIndexedInstanced(3, 1, 0, 0, 0);

    // ------------- DEBUG DRAW ----------------------

    dd::xzSquareGrid(-10.0f, 10.0f, 0.0f, 1.0f, dd::colors::LightGray);
    dd::axisTriad(Chiron::Utils::ddConvert(Matrix::Identity), 0.1f, 1.0f);

    char lTmp[1024];
    sprintf_s(lTmp, 1023, "FPS: [%d].", static_cast<uint32_t>(App->GetFPS()));
    dd::screenText(lTmp, Chiron::Utils::ddConvert(Vector3(10.0f, 10.0f, 0.0f)), dd::colors::White, 0.6f);

    _debugDraw->record(_drawCommandList.Get(), width, height, view, proj);

    d3d12->CreateTransitionBarrier(_drawCommandList, d3d12->GetRenderBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_PRESENT);

    uint64_t fenceValue = d3d12->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT)->ExecuteCommandList(_drawCommandList);
    d3d12->SaveCurrentBufferFenceValue(fenceValue);

    _drawCommandList = nullptr;

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
