#include "Pch.h"
#include "SceneWindow.h"

#include "Application.h"

#include "DataModels/DX12/CommandList/CommandList.h"

#include "DataModels/DX12/DescriptorAllocator/DescriptorAllocator.h"
#include "DataModels/DX12/Resource/Texture.h"

#include "Modules/ModuleRender.h"
#include "DataModels/Assets/ModelAsset.h"
#include "DataModels/Assets/MaterialAsset.h"
#include "DataModels/Assets/TextureAsset.h"

SceneWindow::SceneWindow() : EditorWindow("Scene")
{
    _flags |= ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_MenuBar;
}

SceneWindow::~SceneWindow()
{
}

