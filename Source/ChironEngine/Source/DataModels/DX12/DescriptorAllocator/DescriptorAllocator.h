#pragma once

#include "DescriptorAllocation.h"

class DescriptorAllocatorPage;

class DescriptorAllocator
{
public:
    DescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptorsPerHeap = 256);
    ~DescriptorAllocator();

    // Allocate contiguous descriptors from the CPU
    DescriptorAllocation Allocate(uint32_t numDescriptors = 1);

    // Release the stale descriptor once the frame is completed.
    void ReleaseStaleDescriptors(uint64_t frameNumber);

private:
    // Create a new heap with a specific number of descriptors.
    std::shared_ptr<DescriptorAllocatorPage> CreateAllocatorPage();

    D3D12_DESCRIPTOR_HEAP_TYPE _heapType;
    uint32_t _numDescriptorsPerHeap;

    std::vector<std::shared_ptr<DescriptorAllocatorPage>> _heapPool;
    // Indices of available heaps in the heap pool.
    std::unordered_set<size_t> _availableHeaps;
};

