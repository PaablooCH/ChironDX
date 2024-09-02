#include "Pch.h"
#include "ModuleRender.h"

#include "Application.h"

#include "ModuleCamera.h"
#include "ModuleID3D12.h"
#include "ModuleProgram.h"
#include "ModuleWindow.h"
#include "ModuleFileSystem.h"

#include "DataModels/DX12/CommandList/CommandList.h"
#include "DataModels/DX12/DescriptorAllocator/DescriptorAllocator.h"
#include "DataModels/DX12/DescriptorAllocator/DescriptorAllocatorPage.h"
#include "DataModels/DX12/RootSignature/RootSignature.h"
#include "DataModels/DX12/Resource/IndexBuffer.h"
#include "DataModels/DX12/Resource/Texture.h"
#include "DataModels/DX12/Resource/VertexBuffer.h"
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
    auto file = App->GetModule<ModuleFileSystem>();
    texture = std::make_shared<Texture>();
    file->Import("Assets/Textures/DuckCM.png", texture);

    auto commandQueue = d3d12->GetID3D12CommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
    _debugDraw = std::make_unique<DebugDrawPass>(d3d12->GetDevice(), commandQueue);
    
    // -------------- VERTEX ---------------------
    
    auto copyCommandList = d3d12->GetCommandList(D3D12_COMMAND_LIST_TYPE_COPY);
    
    // Define the geometry for a triangle.
    Vertex triangleVertices[] =
    {
        { { -0.5f,  0.5f, 0.0f }, { 0.0f, 0.0f } },
        { {  0.5f,  0.5f, 0.0f }, { 1.0f, 0.0f } },
        { { -0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f } },
        { {  0.5f, -0.5f, 0.0f }, { 1.0f, 1.0f } }
    };

    const UINT vertexBufferSize = sizeof(triangleVertices);

    vertexBuffer = std::make_shared<VertexBuffer>(CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize), 
        _countof(triangleVertices), sizeof(Vertex), L"Triangle Vertex Buffer");

    D3D12_SUBRESOURCE_DATA subresourceData = {};
    subresourceData.pData = triangleVertices;
    subresourceData.RowPitch = vertexBufferSize;
    subresourceData.SlicePitch = subresourceData.RowPitch;
    copyCommandList->UpdateBufferResource(vertexBuffer, 0, 1, &subresourceData);

    // -------------- INDEX ---------------------

    uint32_t indexBufferData[6] = { 0, 1, 2, 1, 3, 2 };

    const UINT indexBufferSize = sizeof(indexBufferData);

    indexBuffer = std::make_shared<IndexBuffer>(CD3DX12_RESOURCE_DESC::Buffer(indexBufferSize), _countof(indexBufferData), 
        DXGI_FORMAT_R32_UINT, L"Triangle Index Buffer");

    D3D12_SUBRESOURCE_DATA subresourceData2 = {};
    subresourceData2.pData = indexBufferData;
    subresourceData2.RowPitch = indexBufferSize;
    subresourceData2.SlicePitch = subresourceData2.RowPitch;
    copyCommandList->UpdateBufferResource(indexBuffer, 0, 1, &subresourceData2);

    auto queueType = copyCommandList->GetType();
    uint64_t initFenceValue = d3d12->ExecuteCommandList(copyCommandList);
    d3d12->WaitForFenceValue(queueType, initFenceValue);

    // Change states
    auto directCommandList = d3d12->GetCommandList(D3D12_COMMAND_LIST_TYPE_DIRECT);
    directCommandList->TransitionBarrier(texture.get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    directCommandList->TransitionBarrier(vertexBuffer.get(), D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
    directCommandList->TransitionBarrier(indexBuffer.get(), D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

    initFenceValue = d3d12->ExecuteCommandList(directCommandList);
    d3d12->WaitForFenceValue(D3D12_COMMAND_LIST_TYPE_DIRECT, initFenceValue);

    return true;
}

UpdateStatus ModuleRender::PreUpdate()
{
    auto d3d12 = App->GetModule<ModuleID3D12>();
    
    _drawCommandList = d3d12->GetCommandList(D3D12_COMMAND_LIST_TYPE_DIRECT);
        
    // Clear Viewport
    FLOAT clearColor[] = { 0.4f, 0.4f, 0.4f, 1.0f }; // Set color

    // send the clear command into the list
    _drawCommandList->ClearRenderTargetView(d3d12->GetRenderBuffer(), clearColor, 0);

    _drawCommandList->ClearDepthStencilView(d3d12->GetDepthStencilBuffer(), D3D12_CLEAR_FLAG_DEPTH, 1.0, 0, 0);

    return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus ModuleRender::Update()
{
    auto d3d12 = App->GetModule<ModuleID3D12>();
    auto programs = App->GetModule<ModuleProgram>();
    auto window = App->GetModule<ModuleWindow>();
    auto camera = App->GetModule<ModuleCamera>();

    Program* defaultP = programs->GetProgram(ProgramType::DEFAULT);

    _drawCommandList->UseProgram(defaultP);

    _drawCommandList->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    _drawCommandList->SetVertexBuffers(0, 1, &vertexBuffer->GetVertexBufferView());
    _drawCommandList->SetIndexBuffer(&indexBuffer->GetIndexBufferView());

    unsigned width;
    unsigned height;
    window->GetWindowSize(width, height);

    D3D12_VIEWPORT viewport{};
    viewport.TopLeftX = viewport.TopLeftY = 0;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.Width = static_cast<float>(width);
    viewport.Height = static_cast<float>(height);

    _drawCommandList->SetViewports(1, viewport);
    _drawCommandList->SetScissorRects(1, _scissor);

    auto rtv = d3d12->GetRenderBuffer()->GetRenderTargetView().GetCPUDescriptorHandle();
    auto dsv = d3d12->GetDepthStencilBuffer()->GetDepthStencilView().GetCPUDescriptorHandle();
    _drawCommandList->SetRenderTargets(1, &rtv, FALSE, &dsv);

    Matrix model = Matrix::Identity;
    Matrix view = camera->GetViewMatrix();
    Matrix proj = camera->GetProjMatrix();

    ModelViewProjection mvp;
    mvp.model = model.Transpose();
    mvp.view = view.Transpose();
    mvp.proj = proj.Transpose();

    _drawCommandList->SetGraphics32BitConstants(0, sizeof(ModelViewProjection) / 4, &mvp);

    // set the descriptor heap
    ID3D12DescriptorHeap* descriptorHeaps[] = { 
        texture->GetShaderResourceView().GetDescriptorAllocatorPage()->GetDescriptorHeap().Get()
    };
    _drawCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
    _drawCommandList->SetGraphicsRootDescriptorTable(1, texture->GetShaderResourceView().GetGPUDescriptorHandle());

    _drawCommandList->DrawIndexed(6, 1, 0, 0, 0);

    // ------------- DEBUG DRAW ----------------------

    dd::xzSquareGrid(-10.0f, 10.0f, 0.0f, 1.0f, dd::colors::LightGray);
    dd::axisTriad(Chiron::Utils::ddConvert(Matrix::Identity), 0.1f, 1.0f);

    char lTmp[1024];
    sprintf_s(lTmp, 1023, "FPS: [%d].", static_cast<uint32_t>(App->GetFPS()));
    dd::screenText(lTmp, Chiron::Utils::ddConvert(Vector3(10.0f, 10.0f, 0.0f)), dd::colors::White, 0.6f);

    _debugDraw->record(_drawCommandList->GetGraphicsCommandList().Get(), width, height, view, proj);

    // ------------- CLOSE COMMANDLIST ----------------------

    _drawCommandList->TransitionBarrier(d3d12->GetRenderBuffer(), D3D12_RESOURCE_STATE_PRESENT);

    uint64_t fenceValue = d3d12->ExecuteCommandList(_drawCommandList);
    d3d12->SaveCurrentBufferFenceValue(fenceValue);

    _drawCommandList = nullptr;

    return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus ModuleRender::PostUpdate()
{
    return UpdateStatus::UPDATE_CONTINUE;
}

bool ModuleRender::CleanUp()
{
    return true;
}
