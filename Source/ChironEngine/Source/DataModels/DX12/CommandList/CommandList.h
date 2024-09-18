#pragma once

class DynamicDescriptorHeap;
class Program;
class Resource;
class ResourceStateTracker;
class RootSignature;
class Texture;
class UploadBuffer;

class CommandList
{
public:
    CommandList(D3D12_COMMAND_LIST_TYPE type);
    ~CommandList();

    // ------------- BARRIERS ----------------------

    /**
     * Transition a resource to a particular state.
     *
     * @param resource The resource to transition.
     * @param stateAfter The state to transition the resource to. The before state is resolved by the resource state tracker.
     * @param subresource The subresource to transition. By default, this is D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES which indicates that all subresources are transitioned to the same state.
     * @param flushBarriers Force flush any barriers. Resource barriers need to be flushed before a command (draw, dispatch, or copy) that expects the resource to be in a particular state can run.
     */
    void TransitionBarrier(const Resource* resource, D3D12_RESOURCE_STATES stateAfter, 
        UINT subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, bool flushBarriers = false);

    void TransitionBarrier(ID3D12Resource* resource, D3D12_RESOURCE_STATES stateAfter,
        UINT subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, bool flushBarriers = false);

    /**
     * Add a UAV barrier to ensure that any writes to a resource have completed before reading from the resource.
     *
     * @param resource The resource to apply the barrier.
     * @param flushBarriers Force flush any barriers. Resource barriers need to be flushed before a command (draw, dispatch, or copy) that expects the resource to be in a particular state can run.
     */
    void UAVBarrier(ID3D12Resource* resource, bool flushBarriers = false);
    void UAVBarrier(const Resource* resource, bool flushBarriers = false);

    /**
     * Add an aliasing barrier to indicate a transition between usages of two different resources that occupy the same space in a heap.
     *
     * @param befResource The resource that currently occupies the heap.
     * @param aftResource The resource that will occupy the space in the heap.
     * @param flushBarriers Force flush any barriers. Resource barriers need to be flushed before a command (draw, dispatch, or copy) that expects the resource to be in a particular state can run.
     */
    void AliasingBarrier(ID3D12Resource* befResource, ID3D12Resource* aftResource, bool flushBarriers = false);
    void AliasingBarrier(const Resource* befResource, const Resource* aftResource, bool flushBarriers = false);

    // ------------- COMMAND LIST ACTIONS ----------------------

    void Close();
    bool Close(const CommandList* pendingCommandList);

    void Reset();

    void CopyResource(ID3D12Resource* dstRes, ID3D12Resource* srcRes);
    void CopyResource(const Resource* dstRes, const Resource* srcRes);
    void UpdateBufferResource(const Resource* resource, uint32_t firstSubresource,
        uint32_t numSubresources, D3D12_SUBRESOURCE_DATA* subresourceData);

    void SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY primitiveTopology);
    void SetVertexBuffers(UINT startSlot, UINT numViews, const D3D12_VERTEX_BUFFER_VIEW* pViews);
    void SetIndexBuffer(const D3D12_INDEX_BUFFER_VIEW* pView);
    void SetViewports(UINT numViewports, const D3D12_VIEWPORT& viewport);
    void SetScissorRects(UINT numRects, const D3D12_RECT& scissorRect);
    void SetRenderTargets(UINT numRenderTargetDescriptors, const D3D12_CPU_DESCRIPTOR_HANDLE* pRenderTargetDescriptors,
        BOOL RTsSingleHandleToDescriptorRange, const D3D12_CPU_DESCRIPTOR_HANDLE* pDepthStencilDescriptor);
    void SetGraphicsRoot32BitConstants(uint32_t rootParameterIndex, uint32_t numConstants, const void* constants, 
        UINT destOffsetIn32BitValues = 0);
    void SetDescriptorHeaps(UINT numHeaps, ID3D12DescriptorHeap* descriptorHeaps[]);
    void SetGraphicsRootDescriptorTable(UINT indexRootDescriptor, D3D12_GPU_DESCRIPTOR_HANDLE gpuDescriptorHandle);
    void UseProgram(Program* program);

    void Draw(uint32_t vertexCount, uint32_t instanceCount = 1, uint32_t startVertex = 0, uint32_t startInstance = 0);
    void DrawIndexed(uint32_t indexCount, uint32_t instanceCount = 1, uint32_t startIndex = 0, int32_t baseVertex = 0, 
        uint32_t startInstance = 0);
    void Dispatch(uint32_t threadGroupCountX, uint32_t threadGroupCountY, uint32_t threadGroupCountZ = 1);

    void ClearRenderTargetView(const Texture* rtv, const FLOAT colorRGBA[4], 
        UINT numRects, const D3D12_RECT* pRects = nullptr);
    void ClearDepthStencilView(const Texture* depthStencil, D3D12_CLEAR_FLAGS clearFlags,
        FLOAT depth, UINT8 stencil, UINT numRects, const D3D12_RECT* pRects = nullptr);

    // ------------- GETTERS ----------------------

    inline ComPtr<ID3D12GraphicsCommandList2> GetGraphicsCommandList() const;
    inline std::shared_ptr<CommandList> GetComputeCommandList() const;
    inline D3D12_COMMAND_LIST_TYPE GetType() const;

    // ------------- SETTERS ----------------------

    // Set a dynamic constant buffer data to an inline descriptor in the root signature.
    void SetGraphicsDynamicConstantBuffer(uint32_t rootParameterIndex, size_t sizeInBytes, const void* bufferData);

    // Set the currently bound descriptor heap. Should only be called by the DynamicDescriptorHeap class.
    void SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, ID3D12DescriptorHeap* heap);

    template<typename T>
    inline void SetGraphicsDynamicConstantBuffer(uint32_t rootParameterIndex, const T& data);

    // Set the SRV on the graphics pipeline.
    void SetShaderResourceView(uint32_t rootParameterIndex, uint32_t descriptorOffset, const Texture* texture,
        D3D12_RESOURCE_STATES stateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE |
        D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, UINT firstSubresource = 0,
        UINT numSubresources = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);

    void SetUnorderedAccessView(uint32_t rootParameterIndex, uint32_t descrptorOffset, const Texture* texture, 
        uint32_t mips = 0, D3D12_RESOURCE_STATES stateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS, 
        UINT firstSubresource = 0, UINT numSubresources = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);

    void BindDescriptorHeaps();

private:

    void FlushResourceBarriers();

    // ------------- COMMAND LIST ACTIONS ----------------------

    void SetPipelineState(ComPtr<ID3D12PipelineState> pipelineState);
    void SetRootSignature(const RootSignature* rootSignature, bool graphic);
    void SetCompute32BitConstants(uint32_t rootParameterIndex, const void* constants, uint32_t numConstants = 1);

    // ------------- TRACKERS ----------------------

    inline void TrackObject(ComPtr<ID3D12Object> object);
    void TrackResource(const Resource* resource);
    inline void TrackResource(ID3D12Resource* resource);
    
    void ReleaseTrackedObjects();

private:
    D3D12_COMMAND_LIST_TYPE _type;
    ComPtr<ID3D12GraphicsCommandList2> _commandList;
    ComPtr<ID3D12CommandAllocator> _commandAllocator;

    // For copy queues in case a compute operation is needed.
    std::shared_ptr<CommandList> _computeCommandList;

    // Currently bound root signature.
    ID3D12RootSignature* _rootSignature;

    // Resource created in an upload heap. Useful for drawing of dynamic geometry
    // or for uploading constant buffer data that changes every draw call.
    std::unique_ptr<UploadBuffer> _uploadBuffer;

    // Track (per command list) the current state of a resource. Also tracks the global state.
    std::unique_ptr<ResourceStateTracker> _resourceStateTracker;

    // The dynamic descriptor heap allows for descriptors to be staged before
    // being committed to the command list. Dynamic descriptors need to be committed before a Draw or Dispatch.
    std::unique_ptr<DynamicDescriptorHeap> _dynamicDescriptorHeap[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];

    // Keep track of the currently bound descriptor heaps. Only change descriptor 
    // heaps if they are different than the currently bound descriptor heaps.
    ID3D12DescriptorHeap* _descriptorHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];

    // Safe a reference to an object until all the operations in the commandList have finished.
    std::vector<ComPtr<ID3D12Object>> _objectsTracker;
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

inline std::shared_ptr<CommandList> CommandList::GetComputeCommandList() const
{
    return _computeCommandList;
}

inline D3D12_COMMAND_LIST_TYPE CommandList::GetType() const
{
    return _type;
}

inline void CommandList::TrackObject(ComPtr<ID3D12Object> object)
{
    _objectsTracker.push_back(object);
}

inline void CommandList::TrackResource(ID3D12Resource* resource)
{
    TrackObject(resource);
}