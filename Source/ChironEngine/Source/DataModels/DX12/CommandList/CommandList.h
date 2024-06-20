#pragma once

class DynamicDescriptorHeap;
class Resource;
class ResourceStateTracker;
class UploadBuffer;

class CommandList
{
public:

    CommandList(D3D12_COMMAND_LIST_TYPE type);
    ~CommandList();

    /**
     * Transition a resource to a particular state.
     *
     * @param resource The resource to transition.
     * @param stateAfter The state to transition the resource to. The before state is resolved by the resource state tracker.
     * @param subresource The subresource to transition. By default, this is D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES which indicates that all subresources are transitioned to the same state.
     * @param flushBarriers Force flush any barriers. Resource barriers need to be flushed before a command (draw, dispatch, or copy) that expects the resource to be in a particular state can run.
     */
    void TransitionBarrier(const Resource& resource, D3D12_RESOURCE_STATES stateAfter, 
        UINT subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, bool flushBarriers = false);

    void CopyResource(Resource& dstRes, const Resource& srcRes);

    // Draw geometry.
    void Draw(uint32_t vertexCount, uint32_t instanceCount = 1, uint32_t startVertex = 0, uint32_t startInstance = 0);

    // ------------- SETTERS ----------------------

    inline ComPtr<ID3D12GraphicsCommandList2> GetGraphicsCommandList() const;

    // ------------- SETTERS ----------------------

    // Set a dynamic constant buffer data to an inline descriptor in the root signature.
    void SetGraphicsDynamicConstantBuffer(uint32_t rootParameterIndex, size_t sizeInBytes, const void* bufferData);

    // Set the currently bound descriptor heap. Should only be called by the DynamicDescriptorHeap class.
    void SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, ID3D12DescriptorHeap* heap);

    template<typename T>
    inline void SetGraphicsDynamicConstantBuffer(uint32_t rootParameterIndex, const T& data);

    // Set the SRV on the graphics pipeline.
    void SetShaderResourceView(uint32_t rootParameterIndex, uint32_t descriptorOffset, const Resource& resource,
        D3D12_RESOURCE_STATES stateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE |
        D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, UINT firstSubresource = 0,
        UINT numSubresources = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
        const D3D12_SHADER_RESOURCE_VIEW_DESC* srv = nullptr);

private:
    D3D12_COMMAND_LIST_TYPE _commandListType;
    ComPtr<ID3D12GraphicsCommandList2> _commandList;
    ComPtr<ID3D12CommandAllocator> _commandAllocator;

    // For copy queues, it may be necessary to generate mips while loading textures.
    // Mips can't be generated on copy queues but must be generated on compute or
    // direct queues. In this case, a Compute command list is generated and executed 
    // after the copy queue is finished uploading the first sub resource.
    std::shared_ptr<CommandList> _computeCommandList;

    // Keep track of the currently bound root signatures to minimize root
    // signature changes.
    ID3D12RootSignature* _rootSignature;

    // Resource created in an upload heap. Useful for drawing of dynamic geometry
    // or for uploading constant buffer data that changes every draw call.
    //std::unique_ptr<UploadBuffer> _uploadBuffer;

    // Resource state tracker is used by the command list to track (per command list)
    // the current state of a resource. The resource state tracker also tracks the 
    // global state of a resource in order to minimize resource state transitions.
    //std::unique_ptr<ResourceStateTracker> _resourceStateTracker;

    // The dynamic descriptor heap allows for descriptors to be staged before
    // being committed to the command list. Dynamic descriptors need to be
    // committed before a Draw or Dispatch.
    //std::unique_ptr<DynamicDescriptorHeap> _dynamicDescriptorHeap[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];

    // Keep track of the currently bound descriptor heaps. Only change descriptor 
    // heaps if they are different than the currently bound descriptor heaps.
    ID3D12DescriptorHeap* _descriptorHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
};

template<typename T>
inline void CommandList::SetGraphicsDynamicConstantBuffer(uint32_t rootParameterIndex, const T& data)
{
    SetGraphicsDynamicConstantBuffer(rootParameterIndex, sizeof(T), &data);
}

inline ComPtr<ID3D12GraphicsCommandList2> CommandList::GetGraphicsCommandList() const
{
    return _commandList;
}

