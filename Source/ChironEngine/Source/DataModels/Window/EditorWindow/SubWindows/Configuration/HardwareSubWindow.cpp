#include "Pch.h"
#include "HardwareSubWindow.h"

#include "Application.h"

#include "Modules/ModuleID3D12.h"

#include <sstream>
#include <iomanip>

HardwareSubWindow::HardwareSubWindow() : SubWindow("Hardware")
{
    auto adapter = App->GetModule<ModuleID3D12>()->GetAdapter();

    DXGI_ADAPTER_DESC3 adapterDesc;
    adapter->GetDesc3(&adapterDesc);
    
    _gpuName = Chiron::Utils::WStringToString(adapterDesc.Description);
    
    switch (adapterDesc.VendorId)
    {
    case 0x10DE: 
        _vendorName = "NVIDIA";
        break;
    case 0x1002:
        _vendorName = "AMD";
        break;
    case 0x8086:
        _vendorName = "Intel";
        break;
    case 0x13B5:
        _vendorName = "ARM";
        break;
    case 0x1414:
        _vendorName = "Microsoft";
        break;
    case 0x5143:
        _vendorName = "Qualcomm";
        break;
    case 0x1AE0:
        _vendorName = "Samsung";
        break;
    default:
        _vendorName = "Could not find Vendor's name";
        break;
    }

    MEMORYSTATUSEX memInfo{};
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);
    
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << (memInfo.ullTotalPhys / (1024.f * 1024.f * 1024.f));
    _ram = ss.str() + " GB";
}

HardwareSubWindow::~HardwareSubWindow()
{
}

void HardwareSubWindow::DrawWindowContent(const std::shared_ptr<CommandList>& commandList)
{
    ImGui::SeparatorText("DirectX");
    if (ImGui::BeginTable("##DirectXTable", 2, ImGuiTableFlags_SizingFixedFit))
    {
        ImGui::TableNextColumn();
        ImGui::Text("DirectX Version:");
        ImGui::TableNextColumn();
        ImGui::TextColored(ImVec4(59.f / 255.f, 186.f / 255.f, 115.f / 255.f, 1.f), "12");
        
        ImGui::TableNextColumn();
        ImGui::Text("DXGI Version:");
        ImGui::TableNextColumn();
        ImGui::TextColored(ImVec4(59.f / 255.f, 186.f / 255.f, 115.f / 255.f, 1.f), "1.6");

        ImGui::EndTable();
    }
    ImGui::SeparatorText("Hardware");
    if (ImGui::BeginTable("##HardwareTable", 2, ImGuiTableFlags_SizingFixedFit))
    {
        ImGui::TableNextColumn();
        ImGui::Text("GPU Brand:");
        ImGui::TableNextColumn();
        ImGui::TextColored(ImVec4(59.f / 255.f, 186.f / 255.f, 115.f / 255.f, 1.f), _vendorName.c_str());

        ImGui::TableNextColumn();
        ImGui::Text("GPU Device:");
        ImGui::TableNextColumn();
        ImGui::TextColored(ImVec4(59.f / 255.f, 186.f / 255.f, 115.f / 255.f, 1.f), _gpuName.c_str());

        ImGui::TableNextColumn();
        ImGui::TableNextColumn();

        ImGui::TableNextColumn();
        ImGui::Text("RAM:");
        ImGui::TableNextColumn();
        ImGui::TextColored(ImVec4(59.f / 255.f, 186.f / 255.f, 115.f / 255.f, 1.f), _ram.c_str());
        
        ImGui::TableNextColumn();
        ImGui::TableNextColumn();

        auto adapter = App->GetModule<ModuleID3D12>()->GetAdapter();

        DXGI_QUERY_VIDEO_MEMORY_INFO vramInfo = {};
        adapter->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &vramInfo);
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2) << (vramInfo.Budget / (1024.f * 1024.f));
        std::string result = ss.str() + " MB";
        ImGui::TableNextColumn();
        ImGui::Text("VRAM:");
        ImGui::TableNextColumn();
        ImGui::TextColored(ImVec4(59.f / 255.f, 186.f / 255.f, 115.f / 255.f, 1.f), result.c_str());

        ss.str("");
        ss.clear();
        ss << std::fixed << std::setprecision(2) << (vramInfo.CurrentUsage / (1024.f * 1024.f));
        result = ss.str() + " MB";
        ImGui::TableNextColumn();
        ImGui::Text("Current VRAM usage:");
        ImGui::TableNextColumn();
        ImGui::TextColored(ImVec4(59.f / 255.f, 186.f / 255.f, 115.f / 255.f, 1.f), result.c_str());

        ss.str("");
        ss.clear();
        ss << std::fixed << std::setprecision(2) << ((vramInfo.Budget - vramInfo.CurrentUsage) / (1024.f * 1024.f));
        result = ss.str() + " MB";
        ImGui::TableNextColumn();
        ImGui::Text("Available VRAM:");
        ImGui::TableNextColumn();
        ImGui::TextColored(ImVec4(59.f / 255.f, 186.f / 255.f, 115.f / 255.f, 1.f), result.c_str());
        
        ImGui::EndTable();
    }
}
