#include "Pch.h"
#include "DescriptorAllocator.h"

#include "DescriptorAllocatorPage.h"

DescriptorAllocator::DescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptorsPerHeap) : 
	_heapType(type), _numDescriptorsPerHeap(numDescriptorsPerHeap)
{
}

DescriptorAllocator::~DescriptorAllocator()
{
    _heapPool.clear();
}

DescriptorAllocation DescriptorAllocator::Allocate(uint32_t numDescriptors)
{
    std::lock_guard<std::mutex> lock(_mutex);

    DescriptorAllocation allocation;

    for (auto availableHeap : _availableHeaps)
    {
        auto& allocatorPage = _heapPool[availableHeap];

        allocation = allocatorPage->Allocate(numDescriptors);

        if (allocatorPage->NumFreeHandles() == 0)
        {
            _availableHeaps.erase(availableHeap);
        }

        // A valid allocation has been found.
        if (!allocation.IsNull())
        {
            break;
        }
    }
    // No available heap could satisfy the requested number of descriptors. Create a new Page
    if (allocation.IsNull())
    {
        _numDescriptorsPerHeap = std::max(_numDescriptorsPerHeap, numDescriptors);
        auto newPage = CreateAllocatorPage();

        allocation = newPage->Allocate(numDescriptors);
    }

    return allocation;
}

void DescriptorAllocator::ReleaseStaleDescriptors(uint64_t frameNumber)
{
    std::lock_guard<std::mutex> lock(_mutex);

    for (size_t i = 0; i < _heapPool.size(); ++i)
    {
        auto& page = _heapPool[i];

        page->ReleaseStaleDescriptors(frameNumber);

        if (page->NumFreeHandles() > 0)
        {
            _availableHeaps.insert(i);
        }
    }
}

std::shared_ptr<DescriptorAllocatorPage> DescriptorAllocator::CreateAllocatorPage()
{
	auto newPage = std::make_shared<DescriptorAllocatorPage>(_heapType, _numDescriptorsPerHeap);

	_heapPool.emplace_back(newPage);
	_availableHeaps.insert(_heapPool.size() - 1);

	return newPage;
}
