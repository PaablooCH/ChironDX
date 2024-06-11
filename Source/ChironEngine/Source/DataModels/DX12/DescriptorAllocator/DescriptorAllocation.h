#pragma once

class DescriptorAllocatorPage;

class DescriptorAllocation
{
public:
    DescriptorAllocation();

    DescriptorAllocation(D3D12_CPU_DESCRIPTOR_HANDLE descriptor, uint32_t numHandles, uint32_t descriptorSize, 
        std::shared_ptr<DescriptorAllocatorPage> page);

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

    // Get a descriptor at a particular offset in the allocation.
    inline D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandle(uint32_t offset = 0) const;

    inline uint32_t GetNumHandles() const;

    inline std::shared_ptr<DescriptorAllocatorPage> GetDescriptorAllocatorPage() const;

private:
    // Free the descriptor back to the heap it came from.
    void Free();

private:
    D3D12_CPU_DESCRIPTOR_HANDLE _descriptorHandle;
    uint32_t _numHandles;
    uint32_t _descriptorSize;

    // Pointer back where this allocation came from.
    std::shared_ptr<DescriptorAllocatorPage> _page;
};

inline D3D12_CPU_DESCRIPTOR_HANDLE DescriptorAllocation::GetDescriptorHandle(uint32_t offset) const
{
    assert(offset < _numHandles);
    return CD3DX12_CPU_DESCRIPTOR_HANDLE(_descriptorHandle, offset, _descriptorSize);
}

inline uint32_t DescriptorAllocation::GetNumHandles() const
{
    return _numHandles;
}

inline std::shared_ptr<DescriptorAllocatorPage> DescriptorAllocation::GetDescriptorAllocatorPage() const
{
    return _page;
}
