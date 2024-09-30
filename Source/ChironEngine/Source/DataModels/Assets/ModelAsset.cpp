#include "Pch.h"
#include "ModelAsset.h"

#include "Application.h"

#include "Modules/ModuleCamera.h"

#include "MeshAsset.h"
#include "MaterialAsset.h"
#include "TextureAsset.h"

#include "DataModels/Camera/Camera.h"

#include "DataModels/DX12/CommandList/CommandList.h"
#include "DataModels/DX12/DescriptorAllocator/DescriptorAllocator.h"
#include "DataModels/DX12/DescriptorAllocator/DescriptorAllocatorPage.h"
#include "DataModels/DX12/Resource/IndexBuffer.h"
#include "DataModels/DX12/Resource/VertexBuffer.h"
#include "DataModels/DX12/Resource/Texture.h"

#include "Structs/ModelAttributes.h"

ModelAsset::ModelAsset() : Asset(AssetType::Model), _isTopLeft(false)
{
}

ModelAsset::~ModelAsset()
{
}

void ModelAsset::Draw(const std::shared_ptr<CommandList>& commandList)
{
    for (int i = 0; i < _meshes.size(); i++)
    {
        commandList->SetVertexBuffers(0, 1, &_meshes[i]->GetVertexBuffer()->GetVertexBufferView());
        commandList->SetIndexBuffer(&_meshes[i]->GetIndexBuffer()->GetIndexBufferView());

        Matrix model = Matrix::Identity;

        ModelAttributes modelAttributes;
        modelAttributes.model = Matrix::Identity;
        modelAttributes.uvCorrector = !_isTopLeft;

        commandList->SetGraphicsRoot32BitConstants(1, sizeof(ModelAttributes) / 4, &modelAttributes);

        auto texture = _materials[i]->GetDiffuse();
        // set the descriptor heap
        ID3D12DescriptorHeap* descriptorHeaps[] = {
            texture->GetTexture()->GetShaderResourceView().GetDescriptorAllocatorPage()->GetDescriptorHeap().Get()
        };
        commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
        commandList->SetGraphicsRootDescriptorTable(2, texture->GetTexture()->GetShaderResourceView().GetGPUDescriptorHandle());

        commandList->DrawIndexed(static_cast<UINT>(_meshes[i]->GetIndexBuffer()->GetNumIndices()));
    }
}