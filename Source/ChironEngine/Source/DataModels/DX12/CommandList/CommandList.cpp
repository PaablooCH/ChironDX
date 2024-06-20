#include "Pch.h"
#include "CommandList.h"

#include "Application.h"

#include "Modules/ModuleID3D12.h"

CommandList::CommandList(D3D12_COMMAND_LIST_TYPE type) : _commandListType(type)
{
    /*auto device = App->GetModule<ModuleID3D12>()->GetDevice();

    Chiron::Utils::ThrowIfFailed(device->CreateCommandAllocator(_commandListType, IID_PPV_ARGS(&_commandAllocator)));

    Chiron::Utils::ThrowIfFailed(device->CreateCommandList(0, _commandListType, _commandAllocator.Get(),
        nullptr, IID_PPV_ARGS(&_commandList)));

    _uploadBuffer = std::make_unique<UploadBuffer>();

    _resourceStateTracker = std::make_unique<ResourceStateTracker>();

    for (int i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i)
    {
        _dynamicDescriptorHeap[i] = std::make_unique<DynamicDescriptorHeap>(static_cast<D3D12_DESCRIPTOR_HEAP_TYPE>(i));
        _descriptorHeaps[i] = nullptr;
    }*/
}

CommandList::~CommandList()
{
}

void CommandList::TransitionBarrier(const Resource& resource, D3D12_RESOURCE_STATES stateAfter, UINT subresource, bool flushBarriers)
{
}

void CommandList::CopyResource(Resource& dstRes, const Resource& srcRes)
{
}

void CommandList::Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t startVertex, uint32_t startInstance)
{
}

void CommandList::SetGraphicsDynamicConstantBuffer(uint32_t rootParameterIndex, size_t sizeInBytes, const void* bufferData)
{
}

void CommandList::SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, ID3D12DescriptorHeap* heap)
{
}

void CommandList::SetShaderResourceView(uint32_t rootParameterIndex, uint32_t descriptorOffset, const Resource& resource, 
    D3D12_RESOURCE_STATES stateAfter, UINT firstSubresource, UINT numSubresources, 
    const D3D12_SHADER_RESOURCE_VIEW_DESC* srv)
{
}
