#include "Pch.h"
#include "ModuleEditor.h"

#include "Application.h"

#include "ModuleID3D12.h"
#include "ModuleRender.h"
#include "ModuleWindow.h"

#include "DataModels/Window/EditorWindow/ConsoleWindow.h"
#include "DataModels/Window/MainMenuWindow.h"

#include "DataModels/DX12/CommandList/CommandList.h"
#include "DataModels/DX12/DescriptorAllocator/DescriptorAllocator.h"
#include "DataModels/DX12/DescriptorAllocator/DescriptorAllocation.h"
#include "DataModels/DX12/DescriptorAllocator/DescriptorAllocatorPage.h"
#include "DataModels/DX12/Resource/Texture.h"

#include "ImGui/imgui.h"
#include "ImGui/ImGuizmo.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_internal.h"
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
    ImGui::GetCurrentContext()->NavWindowingToggleLayer = false;
    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(App->GetModule<ModuleWindow>()->GetWindowId());

    auto d3d12 = App->GetModule<ModuleID3D12>();
    _srvDescHeap = std::make_unique<DescriptorAllocation>(
        d3d12->GetDescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)->Allocate(NUM_FRAMES_IN_FLIGHT));
    ImGui_ImplDX12_Init(d3d12->GetDevice(), NUM_FRAMES_IN_FLIGHT, DXGI_FORMAT_R8G8B8A8_UNORM,
        _srvDescHeap->GetDescriptorAllocatorPage()->GetDescriptorHeap().Get(),
        _srvDescHeap->GetCPUDescriptorHandle(), _srvDescHeap->GetGPUDescriptorHandle(),
        _srvDescHeap->GetCPUDescriptorHandle(1), _srvDescHeap->GetGPUDescriptorHandle(1));

    _mainMenu = std::make_unique<MainMenuWindow>();
    _windows.push_back(std::make_unique<ConsoleWindow>());

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

    ImGuiWindowFlags dockSpaceWindowFlags = 0;
    dockSpaceWindowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking;
    dockSpaceWindowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::PopStyleVar(3);
    ImGui::Begin("DockSpace", nullptr, dockSpaceWindowFlags);
    ImGuiID dockSpaceId = ImGui::GetID("DockSpace");
    ImGui::DockSpace(dockSpaceId);

    static bool firstTime = true;
    if (firstTime)
    {
        firstTime = false;

        ImGui::DockBuilderRemoveNode(dockSpaceId);
        ImGui::DockBuilderAddNode(dockSpaceId, dockSpaceWindowFlags | ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockSpaceId, ImGui::GetMainViewport()->Size);

        ImGuiID dockIdUp = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Up, 0.06f, nullptr, &dockSpaceId);
        ImGuiID dockIdRight = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Right, 0.27f, nullptr, &dockSpaceId);
        ImGuiID dockIdDown = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Down, 0.32f, nullptr, &dockSpaceId);
        ImGuiID dockIdLeft = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Left, 0.22f, nullptr, &dockSpaceId);
        ImGui::DockBuilderDockWindow("Console", dockIdDown);
        ImGui::DockBuilderDockWindow("File Browser", dockIdDown);
        ImGui::DockBuilderDockWindow("State Machine Editor", dockIdDown);
        ImGui::DockBuilderDockWindow("Configuration", dockIdRight);
        ImGui::DockBuilderDockWindow("About", dockIdRight);
        ImGui::DockBuilderDockWindow("Navigation", dockIdRight);
        ImGui::DockBuilderDockWindow("Resources", dockIdRight);
        ImGui::DockBuilderDockWindow("Inspector", dockIdRight);
        ImGui::DockBuilderDockWindow("Editor Control", dockIdUp);
        ImGui::DockBuilderDockWindow("Hierarchy", dockIdLeft);
        ImGui::DockBuilderDockWindow("Scene", dockSpaceId);
        ImGui::DockBuilderFinish(dockSpaceId);
    }
    ImGui::End();

    _mainMenu->Draw();

    for (std::unique_ptr<Window>& window : _windows)
    {
        CHIRON_TODO("Change this");
        bool canDraw = true;
        window->Draw(canDraw);
    }

    ImGui::Render();

    if (drawCommandList)
    {
        ID3D12DescriptorHeap* descriptorHeaps[] = {
            _srvDescHeap->GetDescriptorAllocatorPage()->GetDescriptorHeap().Get()
        };
        drawCommandList->SetDescriptorHeaps(1, descriptorHeaps);
        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), drawCommandList->GetGraphicsCommandList().Get());
    }

    drawCommandList->TransitionBarrier(d3d12->GetRenderBuffer(), D3D12_RESOURCE_STATE_PRESENT);

    uint64_t fenceValue = d3d12->ExecuteCommandList(drawCommandList);
    d3d12->SaveCurrentBufferFenceValue(fenceValue);

    return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus ModuleEditor::PostUpdate()
{
    ImGui::Render();
    return UpdateStatus::UPDATE_CONTINUE;
}