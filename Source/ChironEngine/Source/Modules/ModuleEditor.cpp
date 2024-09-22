#include "Pch.h"
#include "ModuleEditor.h"

#include "Application.h"

#include "ModuleID3D12.h"
#include "ModuleWindow.h"

#include "DataModels/Window/EditorWindow/ConsoleWindow.h"
#include "DataModels/Window/EditorWindow/SceneWindow.h"
#include "DataModels/Window/MainMenuWindow.h"

#include "DataModels/DX12/CommandList/CommandList.h"
#include "DataModels/DX12/DescriptorAllocator/DescriptorAllocator.h"
#include "DataModels/DX12/DescriptorAllocator/DescriptorAllocation.h"
#include "DataModels/DX12/DescriptorAllocator/DescriptorAllocatorPage.h"
#include "DataModels/DX12/Resource/Texture.h"

#include "ImGui/ImGuizmo.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/imgui_impl_dx12.h"
#include "ImGui/imgui_impl_win32.h"

#include "ModuleRender.h"
#include "DataModels/Assets/ModelAsset.h"
#include "DataModels/Assets/MaterialAsset.h"
#include "DataModels/Assets/TextureAsset.h"

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
        d3d12->GetDescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)->Allocate());
    ImGui_ImplDX12_Init(d3d12->GetDevice(), NUM_FRAMES_IN_FLIGHT, DXGI_FORMAT_R8G8B8A8_UNORM,
        _srvDescHeap->GetDescriptorAllocatorPage()->GetDescriptorHeap().Get(),
        _srvDescHeap->GetCPUDescriptorHandle(), _srvDescHeap->GetGPUDescriptorHandle());

    _mainMenu = std::make_unique<MainMenuWindow>();
    _windows.push_back(std::make_unique<SceneWindow>());
    _windows.push_back(std::make_unique<ConsoleWindow>());

    SetStyles();

    return true;
}

bool ModuleEditor::Start()
{
    ApplyTheme(_darkCyanStyle);
  
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

    auto drawCommandList = d3d12->GetCommandList(D3D12_COMMAND_LIST_TYPE_DIRECT);

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
    ImGui::ShowDemoWindow();
    ImGui::ShowMetricsWindow();

    for (std::unique_ptr<Window>& window : _windows)
    {
        CHIRON_TODO("Change this");
        bool canDraw = true;
        window->Draw(canDraw, drawCommandList);
    }
    
    drawCommandList->TransitionBarrier(d3d12->GetRenderBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET);

    ImGui::Render();
    
    ID3D12DescriptorHeap* descriptorHeaps[] = {
        _srvDescHeap->GetDescriptorAllocatorPage()->GetDescriptorHeap().Get()
    };
    drawCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), drawCommandList->GetGraphicsCommandList().Get());

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

void ModuleEditor::SetStyles()
{
    // ------------- COLORFULL STYLE ----------------------

    _colorfullStyle.Text = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);  // Color del texto
    _colorfullStyle.TextDisabled = ImVec4(0.36f, 0.42f, 0.47f, 1.00f);  // Texto deshabilitado
    _colorfullStyle.WindowBg = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);  // Fondo de ventana
    _colorfullStyle.ChildBg = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);  // Fondo de contenedor
    _colorfullStyle.PopupBg = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);  // Fondo de popup
    _colorfullStyle.Border = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);  // Color del borde
    _colorfullStyle.FrameBg = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);  // Fondo de los frames
    _colorfullStyle.FrameBgHovered = ImVec4(0.12f, 0.20f, 0.28f, 1.00f);  // Frame al pasar el mouse
    _colorfullStyle.FrameBgActive = ImVec4(0.09f, 0.12f, 0.14f, 1.00f);  // Frame activo
    _colorfullStyle.TitleBg = ImVec4(0.09f, 0.12f, 0.14f, 0.65f);  // Título de la ventana
    _colorfullStyle.TitleBgActive = ImVec4(0.10f, 0.14f, 0.18f, 1.00f);  // Título activo
    _colorfullStyle.MenuBarBg = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);  // Fondo de la barra de menú
    _colorfullStyle.ScrollbarBg = ImVec4(0.02f, 0.02f, 0.02f, 0.39f);  // Fondo del scrollbar
    _colorfullStyle.ScrollbarGrab = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);  // Scrollbar
    _colorfullStyle.ScrollbarGrabHovered = ImVec4(0.18f, 0.22f, 0.25f, 1.00f);  // Scrollbar al pasar el mouse
    _colorfullStyle.ScrollbarGrabActive = ImVec4(0.15f, 0.19f, 0.21f, 1.00f);  // Scrollbar activo
    _colorfullStyle.CheckMark = ImVec4(0.00f, 0.80f, 0.80f, 1.00f);  // Checkmark
    _colorfullStyle.SliderGrab = ImVec4(0.00f, 0.64f, 0.92f, 1.00f);  // Deslizador
    _colorfullStyle.SliderGrabActive = ImVec4(0.00f, 0.84f, 1.00f, 1.00f);  // Deslizador activo
    _colorfullStyle.Button = ImVec4(0.20f, 0.52f, 0.67f, 1.00f);  // Botón
    _colorfullStyle.ButtonHovered = ImVec4(0.25f, 0.62f, 0.82f, 1.00f);  // Botón al pasar el mouse
    _colorfullStyle.ButtonActive = ImVec4(0.12f, 0.35f, 0.58f, 1.00f);  // Botón activo
    _colorfullStyle.Header = ImVec4(0.15f, 0.30f, 0.60f, 1.00f);  // Header
    _colorfullStyle.HeaderHovered = ImVec4(0.20f, 0.40f, 0.72f, 1.00f);  // Header al pasar el mouse
    _colorfullStyle.HeaderActive = ImVec4(0.22f, 0.45f, 0.85f, 1.00f);  // Header activo
    _colorfullStyle.ResizeGrip = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);  // Resize grip
    _colorfullStyle.ResizeGripHovered = ImVec4(0.12f, 0.48f, 0.88f, 1.00f);  // Resize grip al pasar el mouse
    _colorfullStyle.ResizeGripActive = ImVec4(0.08f, 0.35f, 0.70f, 1.00f);  // Resize grip activo

    // ------------- MINIMALIST STYLE ----------------------

    _minimalistStyle.Text = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);  // Texto blanco/gris claro
    _minimalistStyle.TextDisabled = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);  // Texto deshabilitado en gris
    _minimalistStyle.WindowBg = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);  // Fondo de ventana negro
    _minimalistStyle.ChildBg = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);  // Fondo de contenedor ligeramente más claro
    _minimalistStyle.PopupBg = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);  // Fondo de popup muy oscuro
    _minimalistStyle.Border = ImVec4(0.40f, 0.40f, 0.40f, 0.50f);  // Borde en gris tenue
    _minimalistStyle.FrameBg = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);  // Fondo de los frames gris oscuro
    _minimalistStyle.FrameBgHovered = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);  // Frame al pasar el mouse gris más claro
    _minimalistStyle.FrameBgActive = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);  // Frame activo
    _minimalistStyle.TitleBg = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);  // Fondo del título
    _minimalistStyle.TitleBgActive = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);  // Título activo más claro
    _minimalistStyle.MenuBarBg = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);  // Fondo de la barra de menú
    _minimalistStyle.ScrollbarBg = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);  // Fondo del scrollbar negro
    _minimalistStyle.ScrollbarGrab = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);  // Scrollbar gris oscuro
    _minimalistStyle.ScrollbarGrabHovered = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);  // Scrollbar al pasar el mouse
    _minimalistStyle.ScrollbarGrabActive = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);  // Scrollbar activo
    _minimalistStyle.CheckMark = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);  // Checkmark blanco/gris claro
    _minimalistStyle.SliderGrab = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);  // Deslizador gris medio
    _minimalistStyle.SliderGrabActive = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);  // Deslizador activo
    _minimalistStyle.Button = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);  // Botón gris oscuro
    _minimalistStyle.ButtonHovered = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);  // Botón al pasar el mouse gris más claro
    _minimalistStyle.ButtonActive = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);  // Botón activo gris
    _minimalistStyle.Header = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);  // Header gris
    _minimalistStyle.HeaderHovered = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);  // Header al pasar el mouse gris claro
    _minimalistStyle.HeaderActive = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);  // Header activo
    _minimalistStyle.ResizeGrip = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);  // Resize grip gris
    _minimalistStyle.ResizeGripHovered = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);  // Resize grip al pasar el mouse
    _minimalistStyle.ResizeGripActive = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);  // Resize grip activo

    // ------------- DARK CYAN STYLE ----------------------

    _darkCyanStyle.WindowBg = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    _darkCyanStyle.ChildBg = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
    _darkCyanStyle.PopupBg = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
    _darkCyanStyle.Border = ImVec4(0.20f, 0.50f, 0.50f, 1.00f);
    _darkCyanStyle.BorderShadow = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    _darkCyanStyle.FrameBg = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    _darkCyanStyle.FrameBgHovered = ImVec4(0.25f, 0.50f, 0.50f, 0.70f);
    _darkCyanStyle.FrameBgActive = ImVec4(0.25f, 0.50f, 0.50f, 1.00f);
    _darkCyanStyle.TitleBg = ImVec4(0.05f, 0.05f, 0.05f, 1.00f);
    _darkCyanStyle.TitleBgActive = ImVec4(0.20f, 0.50f, 0.50f, 1.00f);
    _darkCyanStyle.TitleBgCollapsed = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
    _darkCyanStyle.ScrollbarBg = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    _darkCyanStyle.ScrollbarGrab = ImVec4(0.20f, 0.50f, 0.50f, 0.60f);
    _darkCyanStyle.ScrollbarGrabHovered = ImVec4(0.20f, 0.50f, 0.50f, 0.80f);
    _darkCyanStyle.ScrollbarGrabActive = ImVec4(0.20f, 0.50f, 0.50f, 1.00f);
    _darkCyanStyle.CheckMark = ImVec4(0.20f, 0.50f, 0.50f, 1.00f);
    _darkCyanStyle.SliderGrab = ImVec4(0.20f, 0.50f, 0.50f, 0.60f);
    _darkCyanStyle.SliderGrabActive = ImVec4(0.20f, 0.50f, 0.50f, 0.90f);
    _darkCyanStyle.Button = ImVec4(0.20f, 0.50f, 0.50f, 0.40f);
    _darkCyanStyle.ButtonHovered = ImVec4(0.20f, 0.50f, 0.50f, 1.00f);
    _darkCyanStyle.ButtonActive = ImVec4(0.20f, 0.50f, 0.50f, 1.00f);
    _darkCyanStyle.Header = ImVec4(0.20f, 0.50f, 0.50f, 0.40f);
    _darkCyanStyle.HeaderHovered = ImVec4(0.20f, 0.50f, 0.50f, 0.80f);
    _darkCyanStyle.HeaderActive = ImVec4(0.20f, 0.50f, 0.50f, 1.00f);
    _darkCyanStyle.Tab = ImVec4(0.20f, 0.50f, 0.50f, 0.60f);
    _darkCyanStyle.TabHovered = ImVec4(0.20f, 0.50f, 0.50f, 0.80f);
    _darkCyanStyle.TabActive = ImVec4(0.20f, 0.50f, 0.50f, 1.00f);
    _darkCyanStyle.TabUnfocused = ImVec4(0.10f, 0.30f, 0.30f, 0.60f);
    _darkCyanStyle.TabUnfocusedActive = ImVec4(0.15f, 0.35f, 0.35f, 1.00f);
    _darkCyanStyle.Separator = ImVec4(0.20f, 0.50f, 0.50f, 0.40f);
    _darkCyanStyle.SeparatorHovered = ImVec4(0.20f, 0.50f, 0.50f, 0.80f);
    _darkCyanStyle.SeparatorActive = ImVec4(0.20f, 0.50f, 0.50f, 1.00f);
    _darkCyanStyle.PlotLines = ImVec4(0.20f, 0.50f, 0.50f, 1.00f);
    _darkCyanStyle.PlotLinesHovered = ImVec4(0.20f, 0.50f, 0.50f, 1.00f);
    _darkCyanStyle.Text = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    _darkCyanStyle.TextDisabled = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    _darkCyanStyle.TextSelectedBg = ImVec4(0.20f, 0.50f, 0.50f, 0.35f);
    _darkCyanStyle.ModalWindowDimBg = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
}

void ModuleEditor::ApplyTheme(const ThemeColors& theme)
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    // Fondo principal en negro
    colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);

    // Bordes y detalles secundarios en cian
    colors[ImGuiCol_Border] = ImVec4(0.20f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

    // Botones y barras
    colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.25f, 0.50f, 0.50f, 0.70f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.25f, 0.50f, 0.50f, 1.00f);

    // Títulos
    colors[ImGuiCol_TitleBg] = ImVec4(0.05f, 0.05f, 0.05f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.20f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);

    // Barras de scroll y checkboxes
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.20f, 0.50f, 0.50f, 0.60f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.20f, 0.50f, 0.50f, 0.80f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.20f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.20f, 0.50f, 0.50f, 1.00f);

    // Sliders y botones
    colors[ImGuiCol_SliderGrab] = ImVec4(0.20f, 0.50f, 0.50f, 0.60f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.20f, 0.50f, 0.50f, 0.90f);
    colors[ImGuiCol_Button] = ImVec4(0.20f, 0.50f, 0.50f, 0.40f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.20f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.50f, 0.50f, 1.00f);

    // Headers y tabs
    colors[ImGuiCol_Header] = ImVec4(0.20f, 0.50f, 0.50f, 0.40f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.20f, 0.50f, 0.50f, 0.80f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.20f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_Tab] = ImVec4(0.20f, 0.50f, 0.50f, 0.60f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.20f, 0.50f, 0.50f, 0.80f);
    colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.10f, 0.30f, 0.30f, 0.60f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.15f, 0.35f, 0.35f, 1.00f);

    // Separadores y gráficos
    colors[ImGuiCol_Separator] = ImVec4(0.20f, 0.50f, 0.50f, 0.40f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.20f, 0.50f, 0.50f, 0.80f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.20f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_PlotLines] = ImVec4(0.20f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.20f, 0.50f, 0.50f, 1.00f);

    // Otros
    colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.20f, 0.50f, 0.50f, 0.35f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
}
