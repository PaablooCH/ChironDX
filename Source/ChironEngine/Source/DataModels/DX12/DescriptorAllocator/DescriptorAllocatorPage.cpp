#include "Pch.h"
#include "DescriptorAllocatorPage.h"

#include "Application.h"

#include "Modules/ModuleID3D12.h"

DescriptorAllocatorPage::DescriptorAllocatorPage(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptorsPerHeap) :
	_heapType(type), _numDescriptorsPerHeap(numDescriptorsPerHeap)
{
    auto device = App->GetModule<ModuleID3D12>()->GetDevice();

    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
    heapDesc.Type = _heapType;
    heapDesc.NumDescriptors = _numDescriptorsPerHeap;

    Chiron::Utils::ThrowIfFailed(device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&_descriptorHeap)));

    _baseDescriptor = _descriptorHeap->GetCPUDescriptorHandleForHeapStart();
    _descriptorSize = device->GetDescriptorHandleIncrementSize(_heapType);
    _numFreeHandles = _numDescriptorsPerHeap;

    // Initialize the free lists
    AddNewBlock(0, _numFreeHandles);
}

DescriptorAllocation DescriptorAllocatorPage::Allocate(uint32_t numDescriptors)
{
    std::lock_guard<std::mutex> lock(_mutex);

    // There are fewer free descriptors than currently needed. Search for another page.
    if (numDescriptors > _numFreeHandles)
    {
        return DescriptorAllocation();
    }

    // Search the first slot that is larger to contain the new memory
    auto blockSizeIt = _freeBlocksBySize.lower_bound(numDescriptors);
    if (blockSizeIt == _freeBlocksBySize.end())
    {
        LOG_INFO("Not enough contiguous space to allocate {}", numDescriptors);
        return DescriptorAllocation();
    }

    auto& blockOffsetIt = blockSizeIt->second;
    uint32_t size = blockSizeIt->first;
    uint32_t offset = blockOffsetIt->first;

    uint32_t newOffset = offset + numDescriptors;
    uint32_t newSize = size - numDescriptors;

    // Delete old info
    _freeBlocksByOffset.erase(blockOffsetIt);
    _freeBlocksBySize.erase(blockSizeIt);

    if (newSize > 0)
    {
        // Add new info
        AddNewBlock(newOffset, newSize);
    }

    // Decrement free handles counter
    _numFreeHandles -= numDescriptors;
    
	return DescriptorAllocation(CD3DX12_CPU_DESCRIPTOR_HANDLE(_baseDescriptor, offset, _descriptorSize),
        numDescriptors, _descriptorSize, GetSharedPtr());
}

bool DescriptorAllocatorPage::HasSpace(uint32_t numDescriptors) const
{
    if (numDescriptors > _numFreeHandles)
    {
        return false;
    }

    return _freeBlocksBySize.lower_bound(numDescriptors) != _freeBlocksBySize.end();
}

void DescriptorAllocatorPage::Free(DescriptorAllocation&& descriptorHandle, uint64_t frameNumber)
{
    uint32_t offset = ComputeOffset(descriptorHandle.GetDescriptorHandle());

    std::lock_guard<std::mutex> lock(_mutex);
    
    _staleDescriptors.emplace(offset, descriptorHandle.GetNumHandles(), frameNumber);
}

void DescriptorAllocatorPage::ReleaseStaleDescriptors(uint64_t frameNumber)
{
    std::lock_guard<std::mutex> lock(_mutex);
    while (!_staleDescriptors.empty() && _staleDescriptors.front().frameNumber <= frameNumber)
    {
        StaleDescriptorInfo& descInfo = _staleDescriptors.front();
        FreeBlock(descInfo.offset, descInfo.size);
        _staleDescriptors.pop();
    }
}

uint32_t DescriptorAllocatorPage::ComputeOffset(D3D12_CPU_DESCRIPTOR_HANDLE handle) const
{
    return static_cast<uint32_t>(handle.ptr - _baseDescriptor.ptr) / _descriptorSize;
}

void DescriptorAllocatorPage::AddNewBlock(uint32_t offset, uint32_t numDescriptors)
{
    auto offsetIt = _freeBlocksByOffset.emplace(offset, numDescriptors);
    auto sizeIt = _freeBlocksBySize.emplace(numDescriptors, offsetIt.first);
    offsetIt.first->second.blockBySizeIt = sizeIt;
}

void DescriptorAllocatorPage::FreeBlock(uint32_t offset, uint32_t numDescriptors)
{
    // Search the first free offset after the given
    auto nextBlockIt = _freeBlocksByOffset.upper_bound(offset);
    auto& prevBlockIt = nextBlockIt;
    if (prevBlockIt != _freeBlocksByOffset.begin())
    {
        --prevBlockIt;
    }
    else
    {
        prevBlockIt = _freeBlocksByOffset.end();
    }

    uint32_t newSize = numDescriptors;
    uint32_t newOffset = offset;
    if (prevBlockIt != _freeBlocksByOffset.end() && offset == prevBlockIt->first + prevBlockIt->second.size)
    {
        // PrevBlock.Offset           Offset
        // |                          |
        // |<-----PrevBlock.Size----->|<------Descriptors-------->|
        //
        newSize += prevBlockIt->second.size;
        newOffset = prevBlockIt->first;

        _freeBlocksBySize.erase(prevBlockIt->second.blockBySizeIt);
        _freeBlocksByOffset.erase(prevBlockIt);
    }
    else if (nextBlockIt != _freeBlocksByOffset.end() && offset + numDescriptors == nextBlockIt->first)
    {
        // Offset                      NextBlock.Offset 
        // |                           |
        // |<------Descriptors-------->|<-----NextBlock.Size----->|
        //
        newSize += nextBlockIt->second.size;

        _freeBlocksBySize.erase(nextBlockIt->second.blockBySizeIt);
        _freeBlocksByOffset.erase(nextBlockIt);
    }

    AddNewBlock(newOffset, newSize);

    _numFreeHandles += numDescriptors;
}
