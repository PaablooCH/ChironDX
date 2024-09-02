#pragma once

class DescriptorAllocatorPage;

class DescriptorAllocation
{
public:
    DescriptorAllocation();

    DescriptorAllocation(D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptor, CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescriptor,
        uint32_t numHandles, uint32_t descriptorSize, std::shared_ptr<DescriptorAllocatorPage> page);

    // The destructor will automatically free the allocation.
    ~DescriptorAllocation();

    // Copies are not allowed.
    DescriptorAllocation(const DescriptorAllocation&) = delete;
    DescriptorAllocation& operator=(const DescriptorAllocation&) = delete;

    // Move is allowed.
    DescriptorAllocation(DescriptorAllocation&& allocation) noexcept;
    DescriptorAllocation& operator=(DescriptorAllocation&& other) noexcept;

    bool IsNull() const;

    // ------------- GETTERS ----------------------

    // Get a CPU descriptor at a particular offset in the allocation.
    inline D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t offset = 0) const;
    // Get a GPU descriptor at a particular offset in the allocation.
    inline D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(uint32_t offset = 0) const;

    inline uint32_t GetNumHandles() const;

    inline DescriptorAllocatorPage* GetDescriptorAllocatorPage() const;

private:
    // Free the descriptor back to the heap it came from.
    void Free();

private:
    D3D12_CPU_DESCRIPTOR_HANDLE _cpuDescriptorHandle;
    D3D12_GPU_DESCRIPTOR_HANDLE _gpuDescriptorHandle;
    uint32_t _numHandles;
    uint32_t _descriptorSize;

    // Pointer back where this allocation came from.
    std::shared_ptr<DescriptorAllocatorPage> _page;
};

inline D3D12_CPU_DESCRIPTOR_HANDLE DescriptorAllocation::GetCPUDescriptorHandle(uint32_t offset) const
{
    assert(offset < _numHandles);
    return CD3DX12_CPU_DESCRIPTOR_HANDLE(_cpuDescriptorHandle, offset, _descriptorSize);
}

inline D3D12_GPU_DESCRIPTOR_HANDLE DescriptorAllocation::GetGPUDescriptorHandle(uint32_t offset) const
{
    assert(offset < _numHandles);
    return CD3DX12_GPU_DESCRIPTOR_HANDLE(_gpuDescriptorHandle, offset, _descriptorSize);
}

inline uint32_t DescriptorAllocation::GetNumHandles() const
{
    return _numHandles;
}

inline DescriptorAllocatorPage* DescriptorAllocation::GetDescriptorAllocatorPage() const
{
    return _page.get();
}
