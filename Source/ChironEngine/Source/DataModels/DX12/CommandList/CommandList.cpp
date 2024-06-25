#include "Pch.h"
#include "CommandList.h"

#include "Application.h"

#include "Modules/ModuleID3D12.h"

#include "DataModels/DX12/DynamicDescriptorHeap/DynamicDescriptorHeap.h"
#include "DataModels/DX12/Resource/Resource.h"
#include "DataModels/DX12/ResourceStateTracker/ResourceStateTracker.h"
#include "DataModels/DX12/RootSignature/RootSignature.h"
#include "DataModels/DX12/UploadBuffer/UploadBuffer.h"
#include "DataModels/Programs/Program.h"

CommandList::CommandList(D3D12_COMMAND_LIST_TYPE type) : _commandListType(type), _rootSignature(nullptr)
{
    auto device = App->GetModule<ModuleID3D12>()->GetDevice();

    Chiron::Utils::ThrowIfFailed(device->CreateCommandAllocator(_commandListType, IID_PPV_ARGS(&_commandAllocator)));

    Chiron::Utils::ThrowIfFailed(device->CreateCommandList(0, _commandListType, _commandAllocator.Get(),
        nullptr, IID_PPV_ARGS(&_commandList)));

    _uploadBuffer = std::make_unique<UploadBuffer>();

    _resourceStateTracker = std::make_unique<ResourceStateTracker>();

    for (int i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i)
    {
        _dynamicDescriptorHeap[i] = std::make_unique<DynamicDescriptorHeap>(static_cast<D3D12_DESCRIPTOR_HEAP_TYPE>(i));
        _descriptorHeaps[i] = nullptr;
    }
}

CommandList::~CommandList()
{
}

void CommandList::TransitionBarrier(const Resource& resource, D3D12_RESOURCE_STATES stateAfter, UINT subresource, 
    bool flushBarriers)
{
    _resourceStateTracker->TransitionResource(resource, stateAfter, subresource);

    if (flushBarriers)
    {
        FlushResourceBarriers();
    }
}

void CommandList::TransitionBarrier(ComPtr<ID3D12Resource> resource, D3D12_RESOURCE_STATES stateAfter, UINT subresource, bool flushBarriers)
{
    _resourceStateTracker->TransitionResource(resource.Get(), stateAfter, subresource);

    if (flushBarriers)
    {
        FlushResourceBarriers();
    }
}

void CommandList::UAVBarrier(const Resource& resource, bool flushBarriers)
{
    _resourceStateTracker->UAVBarrier(&resource);

    if (flushBarriers)
    {
        FlushResourceBarriers();
    }
}

void CommandList::AliasingBarrier(const Resource& befResource, const Resource& aftResource, bool flushBarriers)
{
    _resourceStateTracker->AliasingBarrier(&befResource, &aftResource);

    if (flushBarriers)
    {
        FlushResourceBarriers();
    }
}

void CommandList::Close()
{
    FlushResourceBarriers();
    _commandList->Close();
    _resourceStateTracker->CommitFinalResourceStates();
}

bool CommandList::Close(CommandList& pendingCommandList)
{
    // Flush any remaining barriers.
    FlushResourceBarriers();

    _commandList->Close();

    // Flush pending resource barriers.
    uint32_t numPendingBarriers = _resourceStateTracker->FlushPendingResourceBarriers(pendingCommandList);
    // Commit the final resource state to the global state.
    _resourceStateTracker->CommitFinalResourceStates();

    return numPendingBarriers > 0;
}

void CommandList::Reset()
{
    Chiron::Utils::ThrowIfFailed(_commandAllocator->Reset());
    Chiron::Utils::ThrowIfFailed(_commandList->Reset(_commandAllocator.Get(), nullptr));

    _resourceStateTracker->Reset();
    _uploadBuffer->Reset();

    ReleaseTrackedObjects();

    for (int i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i)
    {
        _dynamicDescriptorHeap[i]->Reset();
        _descriptorHeaps[i] = nullptr;
    }

    _rootSignature = nullptr;
    _computeCommandList = nullptr;
}

void CommandList::CopyResource(Resource& dstRes, const Resource& srcRes)
{
    TransitionBarrier(srcRes, D3D12_RESOURCE_STATE_COPY_SOURCE);
    TransitionBarrier(dstRes, D3D12_RESOURCE_STATE_COPY_DEST);

    FlushResourceBarriers();

    _commandList.Get()->CopyResource(dstRes.GetResource(), srcRes.GetResource());

    TrackResource(srcRes);
    TrackResource(dstRes);
}

void CommandList::SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY primitiveTopology)
{
    _commandList->IASetPrimitiveTopology(primitiveTopology);
}

void CommandList::SetVertexBuffers(UINT startSlot, UINT numViews, const D3D12_VERTEX_BUFFER_VIEW* pViews)
{
    _commandList->IASetVertexBuffers(startSlot, numViews, pViews);
}

void CommandList::SetIndexBuffer(const D3D12_INDEX_BUFFER_VIEW* pView)
{
    _commandList->IASetIndexBuffer(pView);
}

void CommandList::SetViewports(UINT numViewports, const D3D12_VIEWPORT& viewport)
{
    _commandList->RSSetViewports(numViewports, &viewport);
}

void CommandList::SetScissorRects(UINT numRects, const D3D12_RECT& scissorRect)
{
    _commandList->RSSetScissorRects(numRects, &scissorRect);
}

void CommandList::SetRenderTargets(UINT numRenderTargetDescriptors, 
    const D3D12_CPU_DESCRIPTOR_HANDLE* pRenderTargetDescriptors, BOOL RTsSingleHandleToDescriptorRange, 
    const D3D12_CPU_DESCRIPTOR_HANDLE* pDepthStencilDescriptor)
{
    _commandList->OMSetRenderTargets(numRenderTargetDescriptors, pRenderTargetDescriptors, 
        RTsSingleHandleToDescriptorRange, pDepthStencilDescriptor);
}

void CommandList::SetGraphics32BitConstants(uint32_t rootParameterIndex, uint32_t numConstants, const void* constants, 
    UINT destOffsetIn32BitValues)
{
    _commandList->SetGraphicsRoot32BitConstants(rootParameterIndex, numConstants, constants, destOffsetIn32BitValues);
}

void CommandList::UseProgram(Program* program)
{
    ID3D12PipelineState* pipelineState = program->GetPipelineState();
    RootSignature* rootSignature = program->GetRootSignature();

    SetPipelineState(pipelineState);
    SetRootSignature(rootSignature);
}

void CommandList::Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t startVertex, uint32_t startInstance)
{
    FlushResourceBarriers();

    for (int i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i)
    {
        _dynamicDescriptorHeap[i]->CommitStagedDescriptorsForDraw(*this);
    }

    _commandList->DrawInstanced(vertexCount, instanceCount, startVertex, startInstance);
}

void CommandList::DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t startIndex, int32_t baseVertex, uint32_t startInstance)
{
    FlushResourceBarriers();

    for (int i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i)
    {
        _dynamicDescriptorHeap[i]->CommitStagedDescriptorsForDraw(*this);
    }

    _commandList->DrawIndexedInstanced(indexCount, instanceCount, startIndex, baseVertex, startInstance);
}

void CommandList::ClearRenderTargetView(const D3D12_CPU_DESCRIPTOR_HANDLE& renderTargetView, const FLOAT colorRGBA[4], UINT numRects, const D3D12_RECT* pRects)
{
    _commandList->ClearRenderTargetView(renderTargetView, colorRGBA, numRects, pRects);
}

void CommandList::ClearDepthStencilView(const D3D12_CPU_DESCRIPTOR_HANDLE& depthStencilView, D3D12_CLEAR_FLAGS clearFlags,
    FLOAT depth, UINT8 stencil, UINT numRects, const D3D12_RECT* pRects)
{
    _commandList->ClearDepthStencilView(depthStencilView, clearFlags, depth, stencil, numRects, pRects);
}

void CommandList::SetGraphicsDynamicConstantBuffer(uint32_t rootParameterIndex, size_t sizeInBytes, const void* bufferData)
{
    auto heapAllocation = _uploadBuffer->Allocate(sizeInBytes, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);

    memcpy(heapAllocation.CPU, bufferData, sizeInBytes);

    _commandList->SetGraphicsRootConstantBufferView(rootParameterIndex, heapAllocation.GPU);
}

void CommandList::SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, ID3D12DescriptorHeap* heap)
{
    if (_descriptorHeaps[heapType] != heap)
    {
        _descriptorHeaps[heapType] = heap;
        BindDescriptorHeaps();
    }
}

void CommandList::SetShaderResourceView(uint32_t rootParameterIndex, uint32_t descriptorOffset, const Resource& resource, 
    D3D12_RESOURCE_STATES stateAfter, UINT firstSubresource, UINT numSubresources, 
    const D3D12_SHADER_RESOURCE_VIEW_DESC* srv)
{
    if (numSubresources != D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES)
    {
        for (UINT i = 0; i < numSubresources; i++)
        {
            TransitionBarrier(resource, stateAfter, firstSubresource + i);
        }
    }
    else
    {
        TransitionBarrier(resource, stateAfter);
    }

    _dynamicDescriptorHeap[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV]->StageDescriptors(rootParameterIndex, 
        descriptorOffset, 1, resource.GetShaderResourceView(srv));

    TrackResource(resource);
}

void CommandList::BindDescriptorHeaps()
{
    UINT numDescriptorHeaps = 0;
    ID3D12DescriptorHeap* descriptorHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES] = {};

    for (uint32_t i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i)
    {
        ID3D12DescriptorHeap* descriptorHeap = _descriptorHeaps[i];
        if (descriptorHeap)
        {
            descriptorHeaps[numDescriptorHeaps++] = descriptorHeap;
        }
    }

    _commandList->SetDescriptorHeaps(numDescriptorHeaps, descriptorHeaps);
}

void CommandList::FlushResourceBarriers()
{
    _resourceStateTracker->FlushResourceBarriers(*this);
}

void CommandList::SetPipelineState(ComPtr<ID3D12PipelineState> pipelineState)
{
    _commandList->SetPipelineState(pipelineState.Get());

    TrackObject(pipelineState);
}

void CommandList::SetRootSignature(const RootSignature* rootSignature)
{
    ID3D12RootSignature* rS = rootSignature->GetID3D12RootSignature();
    if (rS != _rootSignature)
    {
        _rootSignature = rootSignature->GetID3D12RootSignature();

        for (int i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; i++)
        {
            _dynamicDescriptorHeap[i]->ParseRootSignature(*rootSignature);
        }
        
        _commandList->SetGraphicsRootSignature(_rootSignature);

        TrackObject(_rootSignature);
    }
}

void CommandList::TrackObject(ComPtr<ID3D12Object> object)
{
    _objectsTracker.push_back(object);
}

void CommandList::TrackResource(const Resource& resource)
{
    TrackObject(resource.GetResource());
}

void CommandList::ReleaseTrackedObjects()
{
    _objectsTracker.clear();
}
