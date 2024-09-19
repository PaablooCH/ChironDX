#include "Pch.h"
#include "ModuleEditor.h"

#include "Application.h"

#include "ModuleID3D12.h"
#include "ModuleRender.h"
#include "ModuleWindow.h"

#include "DataModels/DX12/CommandList/CommandList.h"
#include "DataModels/DX12/DescriptorAllocator/DescriptorAllocator.h"
#include "DataModels/DX12/DescriptorAllocator/DescriptorAllocation.h"
#include "DataModels/DX12/DescriptorAllocator/DescriptorAllocatorPage.h"
#include "DataModels/DX12/Resource/Texture.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx12.h"

ModuleEditor::ModuleEditor()
{
}

ModuleEditor::~ModuleEditor()
{
}

bool ModuleEditor::Init()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;	// Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;		// Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange; // Prevent mouse flickering
    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(App->GetModule<ModuleWindow>()->GetWindowId());
    
    auto d3d12 = App->GetModule<ModuleID3D12>();
    _srvDescHeap = std::make_unique<DescriptorAllocation>(
        d3d12->GetDescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)->Allocate(NUM_FRAMES_IN_FLIGHT));
    ImGui_ImplDX12_Init(d3d12->GetDevice(), NUM_FRAMES_IN_FLIGHT, DXGI_FORMAT_R8G8B8A8_UNORM, 
        nullptr,
        _srvDescHeap->GetCPUDescriptorHandle(), _srvDescHeap->GetGPUDescriptorHandle(),
        _srvDescHeap->GetCPUDescriptorHandle(1), _srvDescHeap->GetGPUDescriptorHandle(1));

    return true;
}

bool ModuleEditor::Start()
{
    return true;
}

bool ModuleEditor::CleanUp()
{
    _srvDescHeap.reset();
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    return true;
}

UpdateStatus ModuleEditor::PreUpdate()
{
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus ModuleEditor::Update()
{
    auto d3d12 = App->GetModule<ModuleID3D12>();
    auto window = App->GetModule<ModuleWindow>();

    auto drawCommandList = d3d12->GetCommandList(D3D12_COMMAND_LIST_TYPE_DIRECT);

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
    drawCommandList->SetScissorRects(1, CD3DX12_RECT(0, 0, LONG_MAX, LONG_MAX));

    auto rtv = d3d12->GetRenderBuffer()->GetRenderTargetView().GetCPUDescriptorHandle();
    auto dsv = d3d12->GetDepthStencilBuffer()->GetDepthStencilView().GetCPUDescriptorHandle();
    drawCommandList->SetRenderTargets(1, &rtv, FALSE, &dsv);



    ImGui::ShowDemoWindow();

    ImGui::Render();
    
    if (drawCommandList)
    {
        ID3D12DescriptorHeap* descriptorHeaps[] = {
            _srvDescHeap->GetDescriptorAllocatorPage()->GetDescriptorHeap().Get()
        };
        drawCommandList->SetDescriptorHeaps(1, descriptorHeaps);
        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), drawCommandList->GetGraphicsCommandList().Get());
    }

    d3d12->ExecuteCommandList(drawCommandList);
    return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus ModuleEditor::PostUpdate()
{
    ImGui::Render();
    return UpdateStatus::UPDATE_CONTINUE;
}
