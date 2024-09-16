#include "Pch.h"
#include "ModuleRender.h"

#include "Application.h"

#include "ModuleCamera.h"
#include "ModuleID3D12.h"
#include "ModuleProgram.h"
#include "ModuleWindow.h"
#include "ModuleFileSystem.h"

#include "DataModels/Camera/Camera.h"

#include "DataModels/Assets/ModelAsset.h"

#include "DataModels/DX12/CommandList/CommandList.h"
#include "DataModels/DX12/RootSignature/RootSignature.h"
#include "DataModels/DX12/Resource/Texture.h"
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
    model = std::make_shared<ModelAsset>();
    file->Import("Assets/Models/Duck.gltf", model);

    auto commandQueue = d3d12->GetID3D12CommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
    _debugDraw = std::make_unique<DebugDrawPass>(d3d12->GetDevice(), commandQueue);

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
    auto moduleCamera = App->GetModule<ModuleCamera>();

    Program* defaultP = programs->GetProgram(ProgramType::DEFAULT);

    _drawCommandList->UseProgram(defaultP);

    _drawCommandList->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    CHIRON_TODO("MOVE TO MODEL");
    /*_drawCommandList->SetVertexBuffers(0, 1, &vertexBuffer->GetVertexBufferView());
    _drawCommandList->SetIndexBuffer(&indexBuffer->GetIndexBufferView());*/

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

    CHIRON_TODO("MOVE TO MODEL");
    // set the descriptor heap
    /*ID3D12DescriptorHeap* descriptorHeaps[] = { 
        texture->GetTexture()->GetShaderResourceView().GetDescriptorAllocatorPage()->GetDescriptorHeap().Get()
    };
    _drawCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
    _drawCommandList->SetGraphicsRootDescriptorTable(1, texture->GetTexture()->GetShaderResourceView().GetGPUDescriptorHandle());

    _drawCommandList->DrawIndexed(6, 1, 0, 0, 0);*/
    model->Draw(_drawCommandList);

    // ------------- DEBUG DRAW ----------------------

    dd::xzSquareGrid(-50.0f, 50.0f, 0.0f, 1.0f, dd::colors::LightGray);
    dd::axisTriad(Chiron::Utils::ddConvert(Matrix::Identity), 0.1f, 1.0f);

    char lTmp[1024];
    sprintf_s(lTmp, 1023, "FPS: [%d].", static_cast<uint32_t>(App->GetFPS()));
    dd::screenText(lTmp, Chiron::Utils::ddConvert(Vector3(10.0f, 10.0f, 0.0f)), dd::colors::White, 0.6f);

    auto camera = moduleCamera->GetCamera();
    Matrix view = camera->GetViewMatrix();
    Matrix proj = camera->GetProjMatrix();

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
    _drawCommandList.reset();
    _debugDraw.reset();
    model.reset();
    _drawCommandList = nullptr;
    model = nullptr;
    
    return true;
}
