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

#include "Structs/MVPStruct.h"

ModelAsset::ModelAsset() : Asset(AssetType::Model)
{
}

ModelAsset::~ModelAsset()
{
}

void ModelAsset::Draw(std::shared_ptr<CommandList> commandList)
{
    auto camera = App->GetModule<ModuleCamera>()->GetCamera();
    Matrix view = camera->GetViewMatrix();
    Matrix proj = camera->GetProjMatrix();
    for (int i = 0; i < _mesh.size(); i++)
    {
        commandList->SetVertexBuffers(0, 1, &_mesh[i]->GetVertexBuffer()->GetVertexBufferView());
        commandList->SetIndexBuffer(&_mesh[i]->GetIndexBuffer()->GetIndexBufferView());

        Matrix model = Matrix::Identity;

        ModelViewProjection mvp;
        mvp.model = model.Transpose();
        mvp.view = view.Transpose();
        mvp.proj = proj.Transpose();

        commandList->SetGraphics32BitConstants(0, sizeof(ModelViewProjection) / 4, &mvp);

        auto texture = _material[i]->GetDiffuse();
        // set the descriptor heap
        ID3D12DescriptorHeap* descriptorHeaps[] = {
            texture->GetTexture()->GetShaderResourceView().GetDescriptorAllocatorPage()->GetDescriptorHeap().Get()
        };
        commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
        commandList->SetGraphicsRootDescriptorTable(1, texture->GetTexture()->GetShaderResourceView().GetGPUDescriptorHandle());

        commandList->DrawIndexed(static_cast<UINT>(_mesh[i]->GetIndexBuffer()->GetNumIndices()), 1, 0, 0, 0);
    }
}
