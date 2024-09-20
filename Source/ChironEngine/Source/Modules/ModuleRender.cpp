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

#include "Structs/ViewProjection.h"

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
    file->Import("Assets/Models/BakerHouse.fbx", model);

    auto commandQueue = d3d12->GetID3D12CommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
    _debugDraw = std::make_unique<DebugDrawPass>(d3d12->GetDevice(), commandQueue);

    return true;
}

UpdateStatus ModuleRender::PreUpdate()
{
    auto d3d12 = App->GetModule<ModuleID3D12>();

    auto drawCommandList = d3d12->GetCommandList(D3D12_COMMAND_LIST_TYPE_DIRECT);

    // Clear Viewport
    FLOAT clearColor[] = { 0.4f, 0.4f, 0.4f, 1.0f }; // Set color

    // send the clear command into the list
    drawCommandList->ClearRenderTargetView(d3d12->GetRenderBuffer(), clearColor, 0);
    drawCommandList->ClearDepthStencilView(d3d12->GetDepthStencilBuffer(), D3D12_CLEAR_FLAG_DEPTH, 1.0, 0, 0);

    d3d12->ExecuteCommandList(drawCommandList);

    return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus ModuleRender::Update()
{
    auto d3d12 = App->GetModule<ModuleID3D12>();
    auto programs = App->GetModule<ModuleProgram>();
    auto window = App->GetModule<ModuleWindow>();
    auto moduleCamera = App->GetModule<ModuleCamera>();

    auto drawCommandList = d3d12->GetCommandList(D3D12_COMMAND_LIST_TYPE_DIRECT);

    Program* defaultP = programs->GetProgram(ProgramType::DEFAULT);

    drawCommandList->UseProgram(defaultP);

    drawCommandList->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    unsigned width;
    unsigned height;
    window->GetWindowSize(width, height);

    D3D12_VIEWPORT viewport{};
    viewport.TopLeftX = viewport.TopLeftY = 0;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.Width = static_cast<float>(width);
    viewport.Height = static_cast<float>(height);

    drawCommandList->SetViewports(1, viewport);
    drawCommandList->SetScissorRects(1, _scissor);

    auto camera = moduleCamera->GetCamera();
    Matrix view = camera->GetViewMatrix();
    Matrix proj = camera->GetProjMatrix();

    ViewProjection vp;
    vp.view = view.Transpose();
    vp.proj = proj.Transpose();
    drawCommandList->SetGraphicsDynamicConstantBuffer(0, vp);

    auto rtv = d3d12->GetRenderBuffer()->GetRenderTargetView().GetCPUDescriptorHandle();
    auto dsv = d3d12->GetDepthStencilBuffer()->GetDepthStencilView().GetCPUDescriptorHandle();
    drawCommandList->SetRenderTargets(1, &rtv, FALSE, &dsv);

    model->Draw(drawCommandList);

    // ------------- DEBUG DRAW ----------------------

    dd::xzSquareGrid(-50.0f, 50.0f, 0.0f, 1.0f, dd::colors::LightGray);
    dd::axisTriad(Chiron::Utils::ddConvert(Matrix::Identity), 0.1f, 1.0f);

    char lTmp[1024];
    sprintf_s(lTmp, 1023, "FPS: [%d].", static_cast<uint32_t>(App->GetFPS()));
    dd::screenText(lTmp, Chiron::Utils::ddConvert(Vector3(10.0f, 10.0f, 0.0f)), dd::colors::White, 0.6f);

    _debugDraw->record(drawCommandList->GetGraphicsCommandList().Get(), width, height, view, proj);

    // ------------- CLOSE COMMANDLIST ----------------------

    d3d12->ExecuteCommandList(drawCommandList);

    return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus ModuleRender::PostUpdate()
{
    App->GetModule<ModuleID3D12>()->PresentAndSwapBuffer();
    return UpdateStatus::UPDATE_CONTINUE;
}

bool ModuleRender::CleanUp()
{
    _debugDraw.reset();
    model.reset();
    model = nullptr;

    return true;
}