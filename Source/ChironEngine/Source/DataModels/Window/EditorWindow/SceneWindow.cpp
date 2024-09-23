#include "Pch.h"
#include "SceneWindow.h"

#include "Application.h"

#include "Modules/ModuleRender.h"

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
    auto sceneTexture = App->GetModule<ModuleRender>()->GetSceneTexture();
    commandList->TransitionBarrier(sceneTexture, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    ImGui::Image((ImTextureID)(sceneTexture->GetShaderResourceView().GetGPUDescriptorHandle().ptr),
        ImGui::GetContentRegionAvail());
}
