#include "Pch.h"
#include "MeshRendererComponent.h"

#include "Application.h"

#include "Modules/ModuleResources.h"

#include "DataModels/Assets/MaterialAsset.h"
#include "DataModels/Assets/MeshAsset.h"
#include "DataModels/Assets/TextureAsset.h"

#include "DataModels/Components/TransformComponent.h"

#include "DataModels/GameObject/GameObject.h"

#include "DataModels/DX12/CommandList/CommandList.h"
#include "DataModels/DX12/DescriptorAllocator/DescriptorAllocatorPage.h"
#include "DataModels/DX12/Resource/IndexBuffer.h"
#include "DataModels/DX12/Resource/Texture.h"
#include "DataModels/DX12/Resource/VertexBuffer.h"

#include "Structs/ModelAttributes.h"

MeshRendererComponent::MeshRendererComponent(GameObject* owner) : Component(ComponentType::MESH_RENDERER, owner)
{
}

MeshRendererComponent::MeshRendererComponent(const MeshRendererComponent& copy) : Component(copy)
{
}

MeshRendererComponent::~MeshRendererComponent()
{
}

void MeshRendererComponent::Render(const std::shared_ptr<CommandList>& commandList) const
{
    if (_mesh && _mesh->GetVertexBuffer() && _mesh->GetIndexBuffer())
    {
        commandList->SetVertexBuffers(0, 1, &_mesh->GetVertexBuffer()->GetVertexBufferView());
        commandList->SetIndexBuffer(&_mesh->GetIndexBuffer()->GetIndexBufferView());
    }
    
    Matrix model = _owner->GetInternalComponent<TransformComponent>()->GetGlobalMatrix();

    ModelAttributes modelAttributes;
    modelAttributes.model = model.Transpose();
    modelAttributes.uvCorrector = isBottomLeft;
    modelAttributes.hasAlbedo = 0;
    
    CHIRON_TODO("CorrectUV for each texture");

    if (_material)
    {
        if (TextureAsset* texture = _material->GetBaseTexture())
        {
            modelAttributes.uvCorrector = texture->GetConfigFlags();
            modelAttributes.hasAlbedo = 1;
            commandList->TransitionBarrier(texture->GetTexture().get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
            // set the descriptor heap
            ID3D12DescriptorHeap* descriptorHeaps[] = {
                texture->GetTexture()->GetShaderResourceView().GetDescriptorAllocatorPage()->GetDescriptorHeap().Get()
            };
            commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
            commandList->SetGraphicsRootDescriptorTable(2, texture->GetTexture()->GetShaderResourceView().GetGPUDescriptorHandle());
        }
    }
    commandList->SetGraphicsRoot32BitConstants(1, sizeof(ModelAttributes) / 4, &modelAttributes);

    if (_mesh && _mesh->GetIndexBuffer())
    {
        commandList->DrawIndexed(static_cast<UINT>(_mesh->GetIndexBuffer()->GetNumIndices()));
    }
}

void MeshRendererComponent::InternalSave(Field& meta)
{
    UID uid = 0;
    if (_material)
    {
        uid = _material->GetUID();
    }
    meta["materialUIDs"] = uid;

    uid = 0;
    if (_mesh)
    {
        uid = _mesh->GetUID();
    }
    meta["meshUIDs"] = uid;
}

void MeshRendererComponent::InternalLoad(const Field& meta)
{
    auto moduleResource = App->GetModule<ModuleResources>();

    UID materialUID = meta["materialUIDs"];
    auto futureMaterial = moduleResource->SearchAsset<MaterialAsset>(materialUID);

    UID meshUID = meta["meshUIDs"];
    auto futureMesh = moduleResource->SearchAsset<MeshAsset>(meshUID);

    _material = futureMaterial.get();
    _mesh = futureMesh.get();
}