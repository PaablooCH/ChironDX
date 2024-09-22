#include "Pch.h"
#include "SceneWindow.h"

#include "Application.h"

#include "Modules/ModuleID3D12.h"

#include "DataModels/DX12/CommandList/CommandList.h"

#include "DataModels/DX12/DescriptorAllocator/DescriptorAllocator.h"
#include "DataModels/DX12/Resource/Texture.h"

SceneWindow::SceneWindow() : EditorWindow("Scene")
{
    _flags |= ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_MenuBar;
}

SceneWindow::~SceneWindow()
{
}

void SceneWindow::DrawWindowContent(const std::shared_ptr<CommandList>& commandList)
{
    auto renderTexture = App->GetModule<ModuleID3D12>()->GetRenderBuffer();
    //commandList->TransitionBarrier(renderTexture, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    ImGui::Text("CPU handle = %p", renderTexture->GetShaderResourceView().GetCPUDescriptorHandle().ptr);
    ImGui::Text("GPU handle = %p", renderTexture->GetShaderResourceView().GetGPUDescriptorHandle().ptr);
    ImGui::Image((ImTextureID)(renderTexture->GetShaderResourceView().GetGPUDescriptorHandle().ptr),
        ImVec2(512, 512));
}
